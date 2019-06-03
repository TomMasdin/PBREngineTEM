
#include "Shader.h"

Shader::Shader(const GLchar *p_VertexPath, const GLchar *p_FragmentPath) {
	std::string vertexCode;
	std::string fragmentCode;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// Ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		// Open files.
		vShaderFile.open(p_VertexPath);
		fShaderFile.open(p_FragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		// Read file's buffer contents into streams.
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		// Close file handlers.
		vShaderFile.close();
		fShaderFile.close();

		// Convert stream into string.
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();

	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n" << e.what() << std::endl;
	}

	const char *vShaderCode = vertexCode.c_str();
	const char *fShaderCode = fragmentCode.c_str();

	// Compile the shaders.
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// Vertex Shader:
	vertex = glCreateShader(GL_VERTEX_SHADER);
	
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);

	// Print compile errors, if any.
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << p_VertexPath << "\n" << std::endl;
	}

	// Fragment Shader:
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);

	// Print compile errors, if any.
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << p_FragmentPath << "\n" << std::endl;
	}

	// Link both shaders, to create a shader program.
	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertex);
	glAttachShader(m_ID, fragment);
	glLinkProgram(m_ID);

	// Print linking errors, if any.
	glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_ID, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << std::endl;
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::Shader(const GLchar *p_VertexPath, const GLchar *p_FragmentPath, const GLchar *p_GeometryPath)
	: m_HasGeometryShader(true) {
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;

	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	std::ifstream gShaderFile;

	// Ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		// Open files.
		vShaderFile.open(p_VertexPath);
		fShaderFile.open(p_FragmentPath);
		gShaderFile.open(p_GeometryPath);

		std::stringstream vShaderStream;
		std::stringstream fShaderStream;
		std::stringstream gShaderStream;

		// Read file's buffer contents into streams.
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		gShaderStream << gShaderFile.rdbuf();

		// Close file handlers.
		vShaderFile.close();
		fShaderFile.close();
		gShaderFile.close();

		// Convert stream into string.
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		geometryCode = gShaderStream.str();

	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ\n" << e.what() << "\n" << std::endl;
	}

	const char *vShaderCode = vertexCode.c_str();
	const char *fShaderCode = fragmentCode.c_str();
	const char *gShaderCode = geometryCode.c_str();

	// Compile the shaders.
	unsigned int vertex;
	unsigned int fragment;
	unsigned int geometry;
	int success;
	char infoLog[512];

	// Vertex Shader:
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, nullptr);
	glCompileShader(vertex);

	// Print compile errors, if any.
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << p_VertexPath << "\n" << std::endl;
	}

	// Fragment Shader:
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, nullptr);
	glCompileShader(fragment);

	// Print compile errors, if any.
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << p_FragmentPath << "\n" << std::endl;
	}

	// Geometry shader:
	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &gShaderCode, nullptr);
	glCompileShader(geometry);

	// Print compile error, if any.
	glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(geometry, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::COMPILCATION_FAILED\n" << p_GeometryPath << "\n" << std::endl;
	}

	// Link the shaders, to create a shader program.
	m_ID = glCreateProgram();
	glAttachShader(m_ID, vertex);
	glAttachShader(m_ID, fragment);
	glAttachShader(m_ID, geometry);
	glLinkProgram(m_ID);

	// Print linking errors, if any.
	glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_ID, 512, nullptr, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << std::endl;
	}
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);
}

void Shader::Use() {
	glUseProgram(m_ID);
}

void Shader::SetBool(const std::string &p_Name, bool p_bValue) const {
	glUniform1i(glGetUniformLocation(m_ID, p_Name.c_str()), static_cast<GLint>(p_bValue));
}

void Shader::SetInt(const std::string &p_Name, int p_iValue) const {
	glUniform1i(glGetUniformLocation(m_ID, p_Name.c_str()), p_iValue);
}

void Shader::SetFloat(const std::string &p_Name, float p_fValue) const {
	glUniform1f(glGetUniformLocation(m_ID, p_Name.c_str()), p_fValue);
}

void Shader::SetVec2(const std::string &p_Name, const glm::vec2 &p_Value) const {
	glUniform2fv(glGetUniformLocation(m_ID, p_Name.c_str()), 1, &p_Value[0]);
}
void Shader::SetVec2(const std::string &p_Name, float p_XValue, float p_YValue) const {
	glUniform2f(glGetUniformLocation(m_ID, p_Name.c_str()), p_XValue, p_YValue);
}


void Shader::SetVec3(const std::string &p_Name, const glm::vec3 &p_Value) const {
	glUniform3fv(glGetUniformLocation(m_ID, p_Name.c_str()), 1, &p_Value[0]);
}
void Shader::SetVec3(const std::string &p_Name, float p_XValue, float p_YValue, float p_ZValue) const {
	glUniform3f(glGetUniformLocation(m_ID, p_Name.c_str()), p_XValue, p_YValue, p_ZValue);
}

void Shader::SetVec4(const std::string &p_Name, const glm::vec4 &p_Value) const {
	glUniform4fv(glGetUniformLocation(m_ID, p_Name.c_str()), 1, &p_Value[0]);
}
void Shader::SetVec4(const std::string &p_Name, float p_XValue, float p_YValue, float p_ZValue, float p_WValue) const {
	glUniform4f(glGetUniformLocation(m_ID, p_Name.c_str()), p_XValue, p_YValue, p_ZValue, p_WValue);
}

void Shader::SetMat2(const std::string &p_Name, const glm::mat2 &p_Mat) const {
	glUniformMatrix2fv(glGetUniformLocation(m_ID, p_Name.c_str()), 1, GL_FALSE, &p_Mat[0][0]);
}

void Shader::SetMat3(const std::string &p_Name, const glm::mat3 &p_Mat) const {
	glUniformMatrix3fv(glGetUniformLocation(m_ID, p_Name.c_str()), 1, GL_FALSE, &p_Mat[0][0]);
}

void Shader::SetMat4(const std::string &p_Name, const glm::mat4 &p_Mat) const {
	glUniformMatrix4fv(glGetUniformLocation(m_ID, p_Name.c_str()), 1, GL_FALSE, &p_Mat[0][0]);
}

unsigned int Shader::GetID() const {
	return m_ID;
}

void Shader::SetID(unsigned int p_ID) {
	m_ID = p_ID;
}

bool Shader::GetHasGeometryShader() {
	return m_HasGeometryShader;
}

void Shader::ErrorChecker()
{
	GLenum e;
	while ((e = glGetError()) != GL_NO_ERROR) {
		switch (e) {
		case GL_INVALID_ENUM:
			std::cout << "\nERROR: GL_INVALID_ENUM\n";
			assert(false);
			break;
		case GL_INVALID_VALUE:
			std::cout << "\nERROR: GL_INVALID_VALUE\n";
			assert(false);
			break;
		case GL_INVALID_OPERATION:
			std::cout << "\nERROR: GL_INVALID_OPERATION\n";
			assert(false);
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cout << "\nERROR: GL_INVALID_FRAMEBUFFER_OPERATION\n";
			assert(false);
			break;
		case GL_OUT_OF_MEMORY:
			std::cout << "\nERROR: GL_OUT_OF_MEMORY\n";
			assert(false);
			break;
		default:
			std::cout << "\nERROR: UNKNOWN\n";
			assert(false);
			break;
		}
	}
}