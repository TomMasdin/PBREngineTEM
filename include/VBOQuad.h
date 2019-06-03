#pragma once
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <vector>



class VBOQuad
{
private:
	unsigned int m_VaoHandle;
	unsigned int m_VboHandle;


public:
	VBOQuad();
	void Render();
	unsigned int GetVaoHandle();
};