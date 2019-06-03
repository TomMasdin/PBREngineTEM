#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <STB_IMAGE/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>
#include <camera.h>
#include <model.h>
#include <Primitives.h>
#include <Light.h>
#include <PBR.h>
#include <SobelPipeline.h>
#include <FrameBufferObject.h>

#include <iostream>
#include <vector>

class RenderEngine
{
private:
	void generateEquirectangle();
	void generateIrradianceMap();
	void generatePrefilterMap();
	void generateBRDF();
	void setupFrameBuffer();
	void setupSkybox();
	void setStaticParams(Shader* p_shader);
	void setDynamicParams(Shader* p_shader);
	unsigned int loadTexture(const char* p_path);
	unsigned int loadHDRTexture(const char* p_path);

	unsigned int m_captureFramebuffer, m_captureRenderbuffer;
	unsigned int m_environmentMap, m_irradianceMap, m_prefilterMap;
	unsigned int m_hdrTexture;
	unsigned int m_brdfTexture;
	unsigned int m_maxMipLevels;
	unsigned int m_screenWidth, m_screenHeight;

	Shader* m_pbrShader;
	Shader* m_pbrUntexturedShader;
	Shader* m_equirectangularShader;
	Shader* m_irradianceShader;
	Shader* m_cubemapShader;
	Shader* m_prefilterShader;
	Shader* m_brdfShader;
	Shader* m_quadShader;

	//Used for sobel testing
	FrameBufferObject* m_SceneBuffer;
	Sobel* m_SobelFiltering;


	Camera* m_sceneCamera;

	//Projection and View
	glm::mat4 m_captureProjectionMatrix;
	glm::mat4 m_projectionMatrix;
	//Different view matrices for each face of the cube
	glm::mat4 m_cubemapViewMatrices[6];

	//Primitives object renderer
	Primitives* m_PrimitiveObject;

	//PBR texture holder - for primitives
	PBR* m_goldTextures;
	PBR* m_brickTextures;
	PBR* m_modelTextures;

	Model* m_model;

	float framebufferSize;

	//Vector of lights
	std::vector<Light> m_Lights;

public:
	RenderEngine(Camera* p_sceneCamera, float p_screenWidth, float p_screenHeight);

	void RenderPBRDemo();
	void Render();
	void RenderCellShaded();
	void CreateLights(glm::vec3 p_position, glm::vec3 p_colour);
	void SetTextureUnits(Shader* p_shader);


};