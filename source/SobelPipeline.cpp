#include <SobelPipeline.h>
#include <FrameBufferObject.h>


Sobel::Sobel(int p_width, int p_height, int p_windowWidth, int p_windowHeight)
{
	m_luminanceShader = new Shader("resources/shaders/illuminance.vs", "resources/shaders/illuminance.fs");
	m_sobelEdgeShader = new Shader("resources/shaders/sobelDetection.vs", "resources/shaders/sobelDetection.fs");
	m_FrameBuffer = new FrameBufferObject(p_width, p_height, p_windowWidth, p_windowHeight);
	m_textureWidth = p_width;
	m_textureHeight = p_height;
}

void Sobel::SetParams()
{
	m_luminanceShader->Use();
	m_luminanceShader->SetInt("screenTexture", 0);

	m_sobelEdgeShader->Use();
	m_sobelEdgeShader->SetInt("screenTexture", 0);
	m_sobelEdgeShader->SetInt("grayscaleTexture", 1);
}

void Sobel::RenderSobel(unsigned int p_ScreenTexture)
{
	SetParams();
	glBindVertexArray(m_ScreenQuad.GetVaoHandle());
	glEnableVertexAttribArray(0);
	glDisable(GL_DEPTH_TEST);

	//Render the scene texture with the sobel edge detection
	glClear(GL_COLOR_BUFFER_BIT);
	m_sobelEdgeShader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, p_ScreenTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_luminanceMap);
	m_ScreenQuad.Render();

	glEnable(GL_DEPTH_TEST);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Sobel::RenderGrayscale(unsigned int p_ScreenTexture)
{
	glBindVertexArray(m_ScreenQuad.GetVaoHandle());
	glEnableVertexAttribArray(0);
	glDisable(GL_DEPTH_TEST);

	//Render the scene texture to a FBO for grayscaling
	m_FrameBuffer->BindFrameBuffer();
	glClear(GL_COLOR_BUFFER_BIT);
	m_luminanceShader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, p_ScreenTexture);
	m_ScreenQuad.Render();
	m_FrameBuffer->UnbindFrameBuffer();

	m_luminanceMap = m_FrameBuffer->GetColourTexture();

	glEnable(GL_DEPTH_TEST);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

unsigned int Sobel::getSobelMap()
{
	return m_sobelEdgeMap;
}

unsigned int Sobel::getLuminanceMap()
{
	return m_luminanceMap;
}
