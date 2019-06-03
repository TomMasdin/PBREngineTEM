#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;
uniform float roughness;

const float PI = 3.14159265359;

//Van Der Corpus without bit operator
float VanDerCorpus(uint n, uint base)
{
	float inverseBase = 1.0 / float(base);
	float denominator = 1.0;
	float result = 0.0;
	
	for(uint i = 0u; i < 32u; i++)
	{
		if(n > 0u)
		{
			denominator = mod(float(n), 2.0);
			result += denominator * inverseBase;
			inverseBase = inverseBase / 2.0;
			n = uint(float(n) / 2.0);
		}
	}
}

// Hammersley Not using bit operator
vec2 HammersleyNoBits(uint i, uint N)
{
	return vec2(float(i)/float(N), VanDerCorpus(i, 2u));
}

// NDF with the GGX NDF from Epic Games
vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
	float a = roughness * roughness;
	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
	
	//Spherical Coords to Cartesian Coords
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;
	
	//Tangent Space to World Space sample vector
	vec3 up = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);
	
	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}


//Shader to sample the specular using a low-discrepancy Hammersley sampler
void main()
{
	vec3 N = normalize(WorldPos);
	vec3 R = N;
	vec3 V = R;
	
	const uint SamplerCount = 1024u;
	float totalWeight = 0.0;
	vec3 prefilteredColor = vec3(0.0);
	
	for(uint i = 0u; i < SamplerCount; i++)
	{
		vec2 Xi = HammersleyNoBits(i, SamplerCount);
		vec3 H = ImportanceSampleGGX(Xi, N, roughness);
		vec3 L = normalize(2.0 * dot(V, H) * H - V);
		
		float NdotL = max(dot(N, L), 0.0);
		if(NdotL > 0.0)
		{
			prefilteredColor += texture(environmentMap, L).rgb * NdotL;
			totalWeight += NdotL;
		}
	}
	prefilteredColor = prefilteredColor / totalWeight;
	
	FragColor = vec4(prefilteredColor, 1.0);
	
}