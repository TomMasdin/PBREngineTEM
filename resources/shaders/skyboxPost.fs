#version 450

in vec3 vsViewDirection;

out vec4 fragment_color;

uniform samplerCube m_environmentMap;

void main()
{
    fragment_color = texture(m_environmentMap, vsViewDirection);
}