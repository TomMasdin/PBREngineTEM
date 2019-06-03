#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <STB_IMAGE/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <Shader.h>


//!< Simple Storage class that holds all the texture information of a PBR object
class PBR
{
private:
	unsigned int m_albedoMap; // ID for the Albedo texture
	unsigned int m_normalMap; // ID for the normal texture
	unsigned int m_metallicMap; // ID for the Metallic Texture
	unsigned int m_roughnessMap; // ID for the roughness Texture
	unsigned int m_ambientOccMap; // ID for the ambient occlusion texture

	unsigned int loadTextures(const char* p_path); // Basic Texture Loader

public:
	PBR();

	void bindTextureUnits(); //!< Binds the pbr textures to units 3-7 (must correspond with the information bound to the shader)

	// Getters and Setters for each texture (Usable at runtime with slight delay)
	void setAlbedo(const char* p_path);
	void setNormal(const char* p_path);
	void setMetallic(const char* p_path);
	void setRoughness(const char* p_path);
	void setAmbientOcc(const char* p_path);

	unsigned int getAlbedo();
	unsigned int getNormal();
	unsigned int getMetallic();
	unsigned int getRoughness();
	unsigned int getAmbientOcc();
};