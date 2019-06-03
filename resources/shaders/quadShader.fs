#version 330 core
out vec4 FragColor;

in vec2 TexCoords;


uniform sampler2D imageTexture;

void main()
{
	vec4 colour = texture(imageTexture, TexCoords);
	FragColor = colour;
}