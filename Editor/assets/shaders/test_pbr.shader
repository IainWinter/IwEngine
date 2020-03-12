#shader Vertex
#version 330

layout(std140, column_major) uniform Camera {
	mat4 viewProj;
	vec4 camPos;
};

uniform mat4 model;

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec2 TexCoords;
out vec3 Normal;
out vec3 CameraPos;
out vec3 WorldPos;

void main() {
	vec4 worldPos = model * vec4(vert, 1);

	TexCoords = uv;
	Normal    = normal;
	CameraPos = camPos.xyz;
	WorldPos  = worldPos.xyz;

	gl_Position = viewProj * worldPos;
}

#shader Fragment
#version 330

#define MAX_POINT_LIGHTS 16
#define MAX_DIRECTIONAL_LIGHTS 4

struct PointLight {
	vec3 Position;
	float Radius;
};

struct DirectionalLight {
	vec3 Position;
	vec3 Direction;
};

layout(std140) uniform Lights {
	vec2 PAD;
	float pointLightCount;
	float directionalLightCount;
	PointLight       pointLights      [MAX_POINT_LIGHTS];
	DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 CameraPos;
in vec3 WorldPos;

out vec4 FragColor;
//
//// World properties
//
//uniform vec3 lightPos;
//uniform float lightRadius;
//uniform float lightPower;

// Material model

uniform vec3 mat_albedo;
uniform float mat_metallic;
uniform float mat_roughness;
uniform float mat_reflectance;

const float PI = 3.14159265359f;

float lerp(
	float a, 
	float b, 
	float w)
{
	return a + w * (b - a);
}

// For gamma correction

vec3 sRGBToLinear(
	vec3 sRGBCol)
{
	vec3 linearRGBLo = sRGBCol / 12.92f;
	vec3 linearRGBHi = pow((sRGBCol + 0.055f) / 1.055f, vec3(2.4f));
	vec3 linearRGB   = (length(sRGBCol) <= 0.04045f) ? linearRGBLo : linearRGBHi;
	return linearRGB;
}

vec3 linearToSRGB(
	vec3 linearCol)
{
	vec3 sRGBLo = linearCol * 12.92f;
	vec3 sRGBHi = (pow(abs(linearCol), vec3(1.0f / 2.4f)) * 1.055f) - 0.055f;
	vec3 sRGB   = (length(linearCol) <= 0.0031308f) ? sRGBLo : sRGBHi;
	return sRGB;
}

// For PBR BFDR

vec3 F_Schlick(
	vec3 f0, 
	float f90, 
	float u)
{ 
	return f0 + (f90 - f0) * pow(1.f - u, 5.f); 
} 

float V_SmithGGXCorrelated(
	float NdotL, 
	float NdotV, 
	float roughness)
{ 
	float alphaG2 = roughness * roughness;
	float Lambda_GGXV = NdotL * sqrt((-NdotV * alphaG2 + NdotV) * NdotV + alphaG2); 
	float Lambda_GGXL = NdotV * sqrt((-NdotL * alphaG2 + NdotL) * NdotL + alphaG2); 
	
	return 0.5f / (Lambda_GGXV + Lambda_GGXL); 
} 

float D_GGX(
	float NdotH, 
	float roughness) 
{ 
	float m2 = roughness * roughness;
	float f = (NdotH * m2 - NdotH) * NdotH + 1; 
	return m2 / (f * f); 
} 

float Fr_DisneyDiffuse(
	float NdotV, 
	float NdotL, 
	float LdotH, 
	float roughness) 
{ 
	float energyBias   = lerp(0,   0.5,        roughness);
	float energyFactor = lerp(1.0, 1.0 / 1.51, roughness);
	vec3 f0            = vec3(1.0f, 1.0f, 1.0f);
	float fd90         = energyBias + 2.0 * LdotH * LdotH * roughness;
	float lightScatter = F_Schlick(f0, fd90, NdotL).r; 
	float viewScatter  = F_Schlick(f0, fd90, NdotV).r; 
	
	return lightScatter * viewScatter * energyFactor; 
}

float getDistanceAtt(
	vec3 unormalizedLightVector,
	float invSqrRadius)
{
	float dist2 = dot(unormalizedLightVector, unormalizedLightVector);
	return clamp(1.0 - dist2 * invSqrRadius, 0.0, 1.0);
}

void main() {
	vec3 albedo = sRGBToLinear(mat_albedo);

	float roughness = pow(mat_roughness, 4);

	vec3  f0  = mix(vec3(0.16f * mat_reflectance * mat_reflectance), albedo, mat_metallic);
	float f90 = clamp(50.0f * dot(f0, vec3(0.33f)), 0.0f, 1.0f);

	vec3 N = normalize(Normal);
	vec3 V = normalize(CameraPos - WorldPos);

	vec3 color = vec3(0);

	for (int i = 0; i < pointLightCount; i++) {
		vec3 L  = pointLights[i].Position - WorldPos;
		vec3 nL = normalize(L);

		float NdotV = abs(dot(N, V));
		vec3  H = normalize(V + nL);
		float LdotH = clamp(dot(nL, H), 0.0f, 1.0f);
		float NdotH = clamp(dot(N, H), 0.0f, 1.0f);
		float NdotL = clamp(dot(N, nL), 0.0f, 1.0f);

		vec3  F = F_Schlick(f0, f90, LdotH);
		float Vis = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
		float D = D_GGX(NdotH, roughness);

		// Specular BRDF
		vec3 Fr = D * F * Vis / PI;

		// Diffuse BRDF 
		float Fd = Fr_DisneyDiffuse(NdotV, NdotL, LdotH, mat_roughness) / PI;
		Fd *= 1.0f - mat_metallic;

		// Light powa
		float attenuation = getDistanceAtt(L, 1 / pow(pointLights[i].Radius, 2));

		color += (albedo * Fd + Fr) * NdotL * attenuation;
	}

	//for (int i = 0; i < directionalLightCount; i++) {
	//	vec3 L = directionalLights[i].Position - WorldPos;
	//	vec3 nL = normalize(L);

	//	float NdotV = abs(dot(N, V));
	//	vec3  H = normalize(V + nL);
	//	float LdotH = clamp(dot(nL, H), 0.0f, 1.0f);
	//	float NdotH = clamp(dot(N, H), 0.0f, 1.0f);
	//	float NdotL = clamp(dot(N, nL), 0.0f, 1.0f);

	//	vec3  F = F_Schlick(f0, f90, LdotH);
	//	float Vis = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
	//	float D = D_GGX(NdotH, roughness);

	//	// Specular BRDF
	//	vec3 Fr = D * F * Vis / PI;

	//	// Diffuse BRDF 
	//	float Fd = Fr_DisneyDiffuse(NdotV, NdotL, LdotH, mat_roughness) / PI;
	//	Fd *= 1.0f - mat_metallic;

	//	// Light powa
	//	float attenuation = getDistanceAtt(L, 1 / pow(pointLights[i].Radius, 2));

	//	color += (albedo * Fd + Fr) * NdotL * attenuation;
	//}

	FragColor = vec4(linearToSRGB(color), 1.0f);
}
