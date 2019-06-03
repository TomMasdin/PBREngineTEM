#include "..\include\PBR.h"

unsigned int PBR::loadTextures(const char * p_path)
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
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

PBR::PBR()
{

}

void PBR::bindTextureUnits()
{
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, m_albedoMap);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, m_normalMap);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, m_metallicMap);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, m_roughnessMap);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, m_ambientOccMap);
}

void PBR::setAlbedo(const char * p_path)
{
	m_albedoMap = loadTextures(p_path);
}

void PBR::setNormal(const char * p_path)
{
	m_normalMap = loadTextures(p_path);
}

void PBR::setMetallic(const char * p_path)
{
	m_metallicMap = loadTextures(p_path);
}

void PBR::setRoughness(const char * p_path)
{
	m_roughnessMap = loadTextures(p_path);
}

void PBR::setAmbientOcc(const char * p_path)
{
	m_ambientOccMap = loadTextures(p_path);
}

unsigned int PBR::getAlbedo()
{
	return m_albedoMap;
}

unsigned int PBR::getNormal()
{
	return m_normalMap;
}

unsigned int PBR::getMetallic()
{
	return m_metallicMap;
}

unsigned int PBR::getRoughness()
{
	return m_roughnessMap;
}

unsigned int PBR::getAmbientOcc()
{
	return m_ambientOccMap;
}
