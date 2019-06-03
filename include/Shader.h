
#pragma once

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Shader {
private:
	// The program ID.
	unsigned int m_ID;

	// Whether there's a geometry shader.
	bool m_HasGeometryShader = false;

public:
	// Constructor reads and builds the shader based on the specified path (Adapted for Geometry Shader)
	Shader(const GLchar *p_VertexPath, const GLchar *p_FragmentPath);
	Shader(const GLchar *p_VertexPath, const GLchar *p_FragmentPath, const GLchar *p_GeometryPath);

	// Use/activate the shader.
	void Use();

	//Error Checker that flags when an error with openGL functions has occured
	void ErrorChecker();

	// Utility uniform functions - Sends information to the shader
	void SetBool(const std::string &p_Name, bool p_bValue) const;
	void SetInt(const std::string &p_Name, int p_iValue) const;
	void SetFloat(const std::string &p_Name, float p_fValue) const;

	void SetVec2(const std::string &p_Name, const glm::vec2 &p_Value) const;
	void SetVec2(const std::string &p_Name, float p_XValue, float p_YValue) const;

	void SetVec3(const std::string &p_Name, const glm::vec3 &p_Value) const;
	void SetVec3(const std::string &p_Name, float p_XValue, float p_YValue, float p_ZValue) const;

	void SetVec4(const std::string &p_Name, const glm::vec4 &p_Value) const;
	void SetVec4(const std::string &p_Name, float p_XValue, float p_YValue, float p_ZValue, float p_WValue) const;

	void SetMat2(const std::string &p_Name, const glm::mat2 &p_Mat) const;
	void SetMat3(const std::string &p_Name, const glm::mat3 &p_Mat) const;
	void SetMat4(const std::string &p_Name, const glm::mat4 &p_Mat) const;

	// Accessor methods for the shader ID.
	unsigned int GetID() const;
	void SetID(unsigned int p_ID);

	bool GetHasGeometryShader();
};