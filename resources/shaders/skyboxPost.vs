#version 450

out vec3 vsViewDirection;

uniform mat4 invView;
uniform mat4 invProj;


void main()
{
	gl_Position = vec4(((gl_VertexID & 1) << 2) - 1, (gl_VertexID & 2) * 2 - 1, 0.0, 1.0);
    vsViewDirection = mat3(invView) * (invProj * gl_Position).xyz;
}