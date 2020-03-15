#shader Vertex
#version 420

#define MAX_DIRECTIONAL_LIGHTS 4

layout(std140, column_major) uniform Camera {
	mat4 viewProj;
	vec4 camPos;
};

layout(std140, column_major) uniform Shadows {
	int shadows_pad1, shadows_pad2, shadows_pad3;

	int directionalLightSpaceCount;

	mat4 directionalLightSpaces[MAX_DIRECTIONAL_LIGHTS];
};

uniform mat4 model;

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec2 TexCoords;
out vec3 Normal;
out vec3 CameraPos;
out vec3 WorldPos;
out vec4 DirectionalLightPos[MAX_DIRECTIONAL_LIGHTS];

void main() {
	vec4 worldPos = model * vec4(vert, 1);

	TexCoords = uv;
	Normal    = normal;
	CameraPos = camPos.xyz;
	WorldPos  = worldPos.xyz;

	for (int i = 0; i < directionalLightSpaceCount; i++) {
		DirectionalLightPos[i] = directionalLightSpaces[i] * worldPos;
	}

	gl_Position = viewProj * worldPos;
}

#shader Fragment
#version 420

#define MAX_POINT_LIGHTS 16
#define MAX_DIRECTIONAL_LIGHTS 4

struct PointLight {
	vec3 Position;
	float Radius;
};

struct DirectionalLight {
	vec3 InvDirection;
};

// might needs pad
layout(std140) uniform Lights{
	int lights_pad1, lights_pad2;

	int pointLightCount;
	int directionalLightCount;

	PointLight       pointLights      [MAX_POINT_LIGHTS];
	DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

in vec2 TexCoords;
in vec3 Normal;
in vec3 CameraPos;
in vec3 WorldPos;
in vec4 DirectionalLightPos[MAX_DIRECTIONAL_LIGHTS];

out vec4 FragColor;

// Material model

uniform vec3  mat_albedo;
uniform float mat_metallic;
uniform float mat_roughness;
uniform float mat_reflectance;

// Shadow maps

uniform float     mat_hasShadowMap;
uniform sampler2D mat_shadowMap;

uniform float       mat_hasShadowMap2;
uniform samplerCube mat_shadowMap2;

// Math Constants

const float PI = 3.14159265359f;

// Math functions

float lerp(
	float a, 
	float b, 
	float w)
{
	return a + w * (b - a);
}

float linstep(
	float l, 
	float h, 
	float v)
{
	return clamp((v - l) / (h - l), 0.0, 1.0);
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

// Shadows

float DirectionalLightShadow(
	vec4 C)
{
	if (mat_hasShadowMap == 0) {
		return 1.0f;
	}

	vec3 coords = (C.xyz / C.w) * 0.5 + 0.5;
	vec2 moments = texture(mat_shadowMap, coords.xy).rg;
	float compare = coords.z;

	if (compare > 1.0) {
		return 1.0;
	}

	float p = step(compare, moments.x);
	float v = max(moments.y - moments.x * moments.x, 0.00002);

	float d = compare - moments.x;
	float pMax = linstep(0.2, 1.0, v / (v + d * d));

	return min(max(p, pMax), 1.0);
}

float PointLightShadow(
	vec3 NegL,
	float R)
{
	if (mat_hasShadowMap2 == 0) {
		return 1.0f;
	}

	float closestDepth = texture(mat_shadowMap2, NegL).r;
	float currentDepth = length(NegL);

	float bias = 0.05;
	float shadow = R * closestDepth > currentDepth - bias ? 1.0 : 0.0;

	return shadow;
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
		vec3  P = pointLights[i].Position;
		float R = pointLights[i].Radius;

		vec3 L  = P - WorldPos;
		vec3 nL = normalize(L);

		float NdotV = abs(dot(N, V));
		vec3  H     = normalize(V + nL);
		float LdotH = clamp(dot(nL, H), 0.0f, 1.0f);
		float NdotH = clamp(dot(N, H),  0.0f, 1.0f);
		float NdotL = clamp(dot(N, nL), 0.0f, 1.0f);

		vec3  F   = F_Schlick(f0, f90, LdotH);
		float Vis = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
		float D   = D_GGX(NdotH, roughness);

		// Specular BRDF
		vec3 Fr = D * F * Vis / PI;

		// Diffuse BRDF 
		float Fd = Fr_DisneyDiffuse(NdotV, NdotL, LdotH, mat_roughness) / PI;
		Fd *= 1.0f - mat_metallic;

		// Light powa
		float attenuation = getDistanceAtt(L, 1 / pow(R, 2));

		color += (albedo * Fd + Fr) * NdotL * attenuation * PointLightShadow(-L, R);
	}
	
	for (int i = 0; i < directionalLightCount; i++) {
		vec3 L = directionalLights[i].InvDirection;
		vec3 nL = normalize(L);

		float NdotV = abs(dot(N, V));
		vec3  H = normalize(V + nL);
		float LdotH = clamp(dot(nL, H), 0.0f, 1.0f);
		float NdotH = clamp(dot(N, H),  0.0f, 1.0f);
		float NdotL = clamp(dot(N, nL), 0.0f, 1.0f);

		vec3  F = F_Schlick(f0, f90, LdotH);
		float Vis = V_SmithGGXCorrelated(NdotV, NdotL, roughness);
		float D = D_GGX(NdotH, roughness);

		// Specular BRDF
		vec3 Fr = D * F * Vis / PI;

		// Diffuse BRDF 
		float Fd = Fr_DisneyDiffuse(NdotV, NdotL, LdotH, mat_roughness) / PI;
		Fd *= 1.0f - mat_metallic;

		color += (albedo * Fd + Fr) * NdotL * DirectionalLightShadow(DirectionalLightPos[i]);
	}
	
	FragColor = vec4(linearToSRGB(color), 1.0f);
}
