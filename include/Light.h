#pragma once

#include <glm/glm.hpp>

//!< Struct that holds all the neccesary information for a light to pass to the shaders
struct Light
{
	Light(glm::vec3 p_position, glm::vec3 p_colour) : m_position(p_position), m_colour(p_colour) {}
	glm::vec3 m_position;
	glm::vec3 m_colour;
};