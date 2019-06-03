#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <STB_IMAGE/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <Shader.h>
#include "VBOQuad.h"



class FrameBufferObject;

class Sobel
{
private:
	Shader * m_luminanceShader; //!< Shader for the luminance grayscaler (unused atm)
	Shader * m_sobelEdgeShader; //!< Shader for the sobel edge detection

	unsigned int m_luminanceMap; //!< ID of the luminance map (after luminanceShader)
	unsigned int m_sobelEdgeMap; //!< ID of the sobel edge detection (after SobelShader)

	unsigned int m_textureWidth, m_textureHeight;//!< Size of the texture input (for sobelShader)

	FrameBufferObject* m_FrameBuffer;
	VBOQuad m_ScreenQuad;
	

public:
	Sobel(int p_width, int p_height, int p_windowHeight, int p_windowWidth); //!< Settings for the sobel framebuffer

	void SetParams(); //!< Set Shader Params
	void RenderSobel(unsigned int p_ScreenTexture); //!< Render the scene getting the screen texture from the screen framebuffer
	void RenderGrayscale(unsigned int p_ScreenTexture); //!< Renders the scene (without skybox) and grayscales the image


	unsigned int getSobelMap(); //!< Getter for the sobel texture
	unsigned int getLuminanceMap(); //!< Getter for the luminance texture
};