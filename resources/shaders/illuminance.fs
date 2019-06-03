#version 330 core
out vec4 FragColor;

in vec2 TexCoords;


uniform sampler2D screenTexture;

void main()
{
	vec3 weights = vec3(0.2125, 0.7154, 0.0721);
	
	float luminance = dot(texture2D(screenTexture, TexCoords).rgb, weights);
	
	FragColor = vec4(vec3(luminance), 1.0);
}