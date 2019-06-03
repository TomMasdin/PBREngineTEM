#version 330 core
out vec4 FragColor;

in vec2 TexCoords;


uniform sampler2D screenTexture;
uniform sampler2D grayscaleTexture;

//Sobel Kernel in X
mat3 sx = mat3
( 
	1.0, 2.0, 1.0, 
	0.0, 0.0, 0.0, 
	-1.0, -2.0, -1.0 
);
//Sobel Kernel in Y
mat3 sy = mat3
( 
	1.0, 0.0, -1.0, 
	2.0, 0.0, -2.0, 
	1.0, 0.0, -1.0 
);

void main()
{
	//The scene's original texture colour
    vec3 diffuse = texture(screenTexture, TexCoords.xy).rgb;
    mat3 I; //Matrix that holds the surrounding pixels for sobel calculations
    for (int i=0; i<3; i++) 
	{
        for (int j=0; j<3; j++) 
		{
            vec3 sample  = texelFetch(grayscaleTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
            I[i][j] = length(sample); 
        }
    }
	//Gradiant in the X (Sobel sample the X values by processing the I matrix by the Sobel kernel)
	float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]);
	//Gradiant in the Y (Sobel sample the Y values by processing the I matrix by the Sobel kernel)	
	float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);
	//Total gradient on the pixel
	float g = sqrt(pow(gx, 2.0)+pow(gy, 2.0));

    // Smoothing the transition between the gradient values
    g = smoothstep(0.1, 0.9, g);
	// Colour for the edges (change for different outline colours)
    vec3 edgeColor = vec3(0, 0, 0);
	// Mix the diffuse and edgeColor by the factor of the gradient
    FragColor = vec4(mix(diffuse, edgeColor, g), 1.);
} 