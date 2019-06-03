#include "RenderEngine.h"

void RenderEngine::generateEquirectangle()
{
	m_equirectangularShader->Use();
	m_equirectangularShader->SetInt("equirectangularMap", 0);
	m_equirectangularShader->SetMat4("projection", m_captureProjectionMatrix);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_hdrTexture);

	glViewport(0, 0, framebufferSize, framebufferSize);
	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFramebuffer);
	for (unsigned int i = 0; i < 6; ++i)
	{
		m_equirectangularShader->SetMat4("view", m_cubemapViewMatrices[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_environmentMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		m_PrimitiveObject->renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void RenderEngine::generateIrradianceMap()
{
	glGenTextures(1, &m_irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFramebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_captureRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	// pbr: solve diffuse integral by convolution to create an irradiance (cube)map.

	m_irradianceShader->Use();
	m_irradianceShader->SetInt("environmentMap", 0);
	m_irradianceShader->SetMat4("projection", m_captureProjectionMatrix);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFramebuffer);
	for (unsigned int i = 0; i < 6; ++i)
	{
		m_irradianceShader->SetMat4("view", m_cubemapViewMatrices[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		m_PrimitiveObject->renderCube();
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderEngine::generatePrefilterMap()
{
	glGenTextures(1, &m_prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // be sure to set minifcation filter to mip_linear 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	//Bind the generate the mipmap texture using the fbo
	m_prefilterShader->Use();
	m_prefilterShader->SetInt("environmentMap", 0);
	m_prefilterShader->SetMat4("projection", m_captureProjectionMatrix);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap);

	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFramebuffer);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth = 128 * std::pow(0.5, mip);
		unsigned int mipHeight = 128 * std::pow(0.5, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, m_captureRenderbuffer);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		m_prefilterShader->SetFloat("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			m_prefilterShader->SetMat4("view", m_cubemapViewMatrices[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_prefilterMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			m_PrimitiveObject->renderCube();
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderEngine::generateBRDF()
{
	glGenTextures(1, &m_brdfTexture);

	glBindTexture(GL_TEXTURE_2D, m_brdfTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	
	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFramebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_captureRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brdfTexture, 0);

	glViewport(0, 0, 512, 512);
	m_brdfShader->Use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_PrimitiveObject->renderQuad();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderEngine::setupFrameBuffer()
{
	
	glGenFramebuffers(1, &m_captureFramebuffer);
	glGenRenderbuffers(1, &m_captureRenderbuffer);

	glBindFramebuffer(GL_FRAMEBUFFER, m_captureFramebuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_captureRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, framebufferSize, framebufferSize);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_captureRenderbuffer);
}

void RenderEngine::setupSkybox()
{
	glGenTextures(1, &m_environmentMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, framebufferSize, framebufferSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (combatting visible dots artifact)
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void RenderEngine::setStaticParams(Shader* p_shader)
{
	m_projectionMatrix = glm::perspective(glm::radians(m_sceneCamera->Zoom), (float)m_screenWidth / (float)m_screenHeight, 0.1f, 100.0f);
	p_shader->Use();
	p_shader->SetMat4("projection", m_projectionMatrix);
	p_shader->SetMat4("invProj", glm::inverse(m_projectionMatrix));
}

void RenderEngine::setDynamicParams(Shader * p_shader)
{
	glm::mat4 view = m_sceneCamera->GetViewMatrix();
	p_shader->Use();
	p_shader->SetMat4("view", view);
	p_shader->SetMat4("invView", glm::inverse(view));
	p_shader->SetVec3("camPos", m_sceneCamera->Position);

}

unsigned int RenderEngine::loadTexture(const char * p_path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(p_path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << p_path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

unsigned int RenderEngine::loadHDRTexture(const char * p_path)
{
	stbi_set_flip_vertically_on_load(true);
	unsigned int textureID;
	int width, height, nrComponents;
	float *data = stbi_loadf(p_path, &width, &height, &nrComponents, 0);
	if (data)
	{
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
		std::cout << "Width: " << width << std::endl;
		std::cout << "Height: " << height << std::endl;
	}
	else
	{
		std::cout << "Failed to load HDR image" << std::endl;
	}

	return textureID;
}

RenderEngine::RenderEngine(Camera* p_sceneCamera, float p_screenWidth, float p_screenHeight)
{
	//Global openGL states
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	//glEnable(GL_CULL_FACE);
	glDepthFunc(GL_LEQUAL); // set depth function to less than AND equal for skybox depth trick.

	m_sceneCamera = p_sceneCamera;
	m_screenWidth = p_screenWidth;
	m_screenHeight = p_screenHeight;
	framebufferSize = 2048;

	//Initialise shaders and objects
	m_pbrShader = new Shader("resources/shaders/pbr.vs", "resources/shaders/pbr.fs");
	m_pbrUntexturedShader = new Shader("resources/shaders/pbruntextured.vs", "resources/shaders/pbruntextured.fs");
	m_equirectangularShader = new Shader("resources/shaders/cubemap.vs", "resources/shaders/equireToCube.fs");
	m_irradianceShader = new Shader("resources/shaders/cubemap.vs", "resources/shaders/irradiancemap.fs");
	m_cubemapShader = new Shader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
	m_prefilterShader = new Shader("resources/shaders/cubemap.vs", "resources/shaders/preFilterConvo.fs");
	m_brdfShader = new Shader("resources/shaders/brdf.vs", "resources/shaders/brdf.fs");
	m_quadShader = new Shader("resources/shaders/quadShader.vs", "resources/shaders/quadShader.fs");
	m_PrimitiveObject = new Primitives();
	m_SceneBuffer = new FrameBufferObject(m_screenWidth, m_screenHeight, m_screenWidth, m_screenHeight);
	m_SobelFiltering = new Sobel(m_screenWidth, m_screenHeight, m_screenWidth, m_screenHeight);

	//Setup the view & projection for the IBL PBR pipeline
	m_captureProjectionMatrix = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	m_cubemapViewMatrices[0] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	m_cubemapViewMatrices[1] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	m_cubemapViewMatrices[2] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_cubemapViewMatrices[3] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
	m_cubemapViewMatrices[4] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
	m_cubemapViewMatrices[5] = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

	SetTextureUnits(m_pbrShader);
	SetTextureUnits(m_pbrUntexturedShader);
	m_pbrUntexturedShader->SetVec3("albedo", 0.5f, 0.0f, 0.0f);
	m_pbrUntexturedShader->SetFloat("ao", 1.0f);

	m_cubemapShader->Use();
	m_cubemapShader->SetInt("environmentMap", 0);

	//Setup PBR textures
	m_goldTextures = new PBR();
	m_goldTextures->setAlbedo("resources/pbr/gold/albedo.png");
	m_goldTextures->setNormal("resources/pbr/gold/normal.png");
	m_goldTextures->setMetallic("resources/pbr/gold/metallic.png");
	m_goldTextures->setRoughness("resources/pbr/gold/roughness.png");
	m_goldTextures->setAmbientOcc("resources/pbr/gold/ao.png");

	m_brickTextures = new PBR();
	m_brickTextures->setAlbedo("resources/pbr/wall/albedo.png");
	m_brickTextures->setNormal("resources/pbr/wall/normal.png");
	m_brickTextures->setMetallic("resources/pbr/wall/metallic.png");
	m_brickTextures->setRoughness("resources/pbr/wall/roughness.png");
	m_brickTextures->setAmbientOcc("resources/pbr/wall/ao.png");

	m_model = new Model("resources/models/cerberus/cerberus.obj");
	m_modelTextures = new PBR();
	m_modelTextures->setAlbedo("resources/models/cerberus/Textures/Cerberus_A.tga");
	m_modelTextures->setNormal("resources/models/cerberus/Textures/Cerberus_N.tga");
	m_modelTextures->setMetallic("resources/models/cerberus/Textures/Cerberus_M.tga");
	m_modelTextures->setRoughness("resources/models/cerberus/Textures/Cerberus_R.tga");
	m_modelTextures->setAmbientOcc("resources/models/cerberus/Textures/Cerberus_AO.tga");


	//Setup Lights
	//CreateLights(glm::vec3(-10.0f, 10.0f, 10.0f),  glm::vec3(300.0f, 300.0f, 300.0f));
	//CreateLights(glm::vec3(10.0f, 10.0f, 10.0f),   glm::vec3(300.0f, 300.0f, 300.0f));
	//CreateLights(glm::vec3(-10.0f, -10.0f, 10.0f), glm::vec3(300.0f, 300.0f, 300.0f));
	//CreateLights(glm::vec3(10.0f, -10.0f, 10.0f),  glm::vec3(300.0f, 300.0f, 300.0f));

	//Setup the main framebuffer, skybox and skybox texture
	setupFrameBuffer();
	//m_hdrTexture = loadHDRTexture("resources/textures/Factory_Catwalk_2k.hdr");
	//m_hdrTexture = loadHDRTexture("resources/textures/night_bridge_8k.hdr");
	//m_hdrTexture = loadHDRTexture("resources/textures/WinterForest_Env.hdr");
	m_hdrTexture = loadHDRTexture("resources/textures/Milkyway_small.hdr");
	setupSkybox();

	//Go through the IBL pbr pipeline
	generateEquirectangle();
	generateIrradianceMap();
	generatePrefilterMap();
	generateBRDF();
	//Set basic parameters for the shaders (projection)
	setStaticParams(m_pbrUntexturedShader);
	setStaticParams(m_pbrShader);
	setStaticParams(m_cubemapShader);
	//Reset the viewport to the window
	glViewport(0, 0, m_screenWidth, m_screenHeight);
}

void RenderEngine::RenderPBRDemo()
{
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	//m_SceneBuffer->BindFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setDynamicParams(m_pbrUntexturedShader);

	//Bind the IBL data textures (correspond to the texture units told to the PBR shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_brdfTexture);

	int nrRows = 7;
	int nrColumns = 7;
	float spacing = 2.5;

	// render rows*column number of spheres with material properties defined by textures (they all have the same material properties)
	glm::mat4 model = glm::mat4(1.0f);
	for (int row = 0; row < nrRows; ++row)
	{
		m_pbrUntexturedShader->SetFloat("metallic", (float)row / (float)nrRows);
		for (int col = 0; col < nrColumns; ++col)
		{
			// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
			// on direct lighting.
			m_pbrUntexturedShader->SetFloat("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(
				(float)(col - (nrColumns / 2)) * spacing,
				(float)(row - (nrRows / 2)) * spacing,
				-2.0f
			));
			m_pbrUntexturedShader->SetMat4("model", model);
			m_PrimitiveObject->renderSphere();
		}
	}

	// Render the lights so we can see where they are in the world
	for (unsigned int i = 0; i < m_Lights.size(); ++i)
	{
		glm::vec3 newPos = m_Lights[i].m_position + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = m_Lights[i].m_position;
		m_pbrUntexturedShader->SetVec3("lightPositions[" + std::to_string(i) + "]", newPos);
		m_pbrUntexturedShader->SetVec3("lightColors[" + std::to_string(i) + "]", m_Lights[i].m_colour);

		model = glm::mat4(1.0f);
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.5f));
		m_pbrUntexturedShader->SetMat4("model", model);
		m_PrimitiveObject->renderSphere();
	}


	// render skybox (render as last to prevent overdraw)
	setDynamicParams(m_cubemapShader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap);
	m_PrimitiveObject->renderCube();

	//m_SceneBuffer->UnbindFrameBuffer();
	//m_SobelFiltering->RenderSobel(m_SceneBuffer->GetColourTexture());
	

}

void RenderEngine::Render()
{
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setDynamicParams(m_pbrShader);
	glm::mat4 model = glm::mat4(1.0f);

	//Bind the IBL data textures (correspond to the texture units told to the PBR shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_brdfTexture);

	m_goldTextures->bindTextureUnits();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0, 0.0, 2.0));
	m_pbrShader->SetMat4("model", model);
	m_PrimitiveObject->renderSphere();

	m_modelTextures->bindTextureUnits();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(3.0, 0.0, 2.0));
	model = glm::scale(model, glm::vec3(0.05f));
	m_pbrShader->SetMat4("model", model);
	m_model->Render(m_pbrShader->GetID());

	// render skybox (render as last to prevent overdraw)
	setDynamicParams(m_cubemapShader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap);

	//glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap); // display irradiance map for debug
	//glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap); // display prefilter map for debug
	
	setDynamicParams(m_cubemapShader);
	m_cubemapShader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap);
	m_PrimitiveObject->renderCube();
	

	// render BRDF map to screen for debug
	//m_brdfShader->Use();
	//m_PrimitiveObject->renderQuad();
}

void RenderEngine::RenderCellShaded()
{
	m_brdfShader->ErrorChecker();
	m_SceneBuffer->BindFrameBuffer();
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setDynamicParams(m_pbrShader);
	glm::mat4 model = glm::mat4(1.0f);

	//Bind the IBL data textures (correspond to the texture units told to the PBR shader
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_brdfTexture);

	m_brickTextures->bindTextureUnits();

	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-3.0, 0.0, 2.0));
	m_pbrShader->SetMat4("model", model);
	m_PrimitiveObject->renderSphere();

	m_modelTextures->bindTextureUnits();
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(3.0, 0.0, 2.0));
	model = glm::scale(model, glm::vec3(0.05f));
	m_pbrShader->SetMat4("model", model);
	m_model->Render(m_pbrShader->GetID());
	m_SceneBuffer->UnbindFrameBuffer();

	//Send the scene texture off for grayscaling
	m_SobelFiltering->RenderGrayscale(m_SceneBuffer->GetColourTexture());

	m_SceneBuffer->BindFrameBuffer();
	setDynamicParams(m_cubemapShader);
	m_cubemapShader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_environmentMap);
	m_PrimitiveObject->renderCube();
	m_SceneBuffer->UnbindFrameBuffer();

	m_SobelFiltering->RenderSobel(m_SceneBuffer->GetColourTexture());

	

	
}

void RenderEngine::CreateLights(glm::vec3 p_position, glm::vec3 p_colour)
{
	m_Lights.push_back(Light(p_position, p_colour));
}

void RenderEngine::SetTextureUnits(Shader * p_shader)
{
	p_shader->Use();
	p_shader->SetInt("irradianceMap", 0);
	p_shader->SetInt("prefilterMap", 1);
	p_shader->SetInt("brdfLUT", 2);
	p_shader->SetInt("albedoMap", 3);
	p_shader->SetInt("normalMap", 4);
	p_shader->SetInt("metallicMap", 5);
	p_shader->SetInt("roughnessMap", 6);
	p_shader->SetInt("aoMap", 7);
}
