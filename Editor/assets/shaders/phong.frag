#version 420

#include shaders/vertex_texture_blend.shader
#include shaders/gamma_correction.shader
#include shaders/lights.shader

in vec3 WorldPos;
in vec3 CameraPos;
in vec2 TexCoords;
in vec3 Normal;
in vec4 Color;

out vec4 FragColor;

// global props

uniform float ambiance;
uniform float blinn;

// material parameters

uniform vec4  mat_baseColor;
uniform float mat_reflectance;
uniform float mat_emissive;
uniform float mat_ao;

uniform float     mat_hasDiffuseMap;
uniform sampler2D mat_diffuseMap;

uniform float     mat_hasDiffuseMap2; // seperate into another shader?
uniform sampler2D mat_diffuseMap2;

uniform float     mat_hasNormalMap;
uniform sampler2D mat_normalMap;

uniform float     mat_hasNormalMap2;
uniform sampler2D mat_normalMap2;

uniform float     mat_hasReflectanceMap;
uniform sampler2D mat_reflectanceMap;

uniform float     mat_hasAoMap;
uniform sampler2D mat_aoMap;

uniform float     mat_hasAlphaMaskMap;
uniform sampler2D mat_alphaMaskMap;

uniform float     mat_hasShadowMap;       // take out of material at some point
uniform sampler2D mat_shadowMap;          // take out of material at some point

uniform float       mat_hasShadowMap2;    // take out of material at some point
uniform samplerCube mat_shadowMap2;       // take out of material at some point

#include shaders/shadows.frag

const float PI = 3.14159265359f;

// Math functions

float lerp(
	float a, 
	float b, 
	float w)
{
	return a + w * (b - a);
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

// attenuation

float getDistanceAtt(
	vec3 unormalizedLightVector,
	float invSqrRadius)
{
	float dist2 = dot(unormalizedLightVector, unormalizedLightVector);
	return clamp(1.0 - dist2 * invSqrRadius, 0.0, 1.0) * 0.1f;
}

// Phong

vec3 BRDF(
	vec3 N, 
	vec3 V, 
	vec3 L, 
	vec3 baseColor,
	float reflectance)
{
	vec3 nL = normalize(L);

	float NdotL = clamp(dot(N,  nL), 0.0f, 1.0f);

	// diffuse
    float diffuse = max(NdotL, 0.0);
    
	// specular
    float spec = 0.0;
    
	//if(blinn == 1.0) {
		//vec3 halfwayDir = normalize(L + V);  
		//spec = pow(max(dot(N, halfwayDir), 0.0), reflectance * 256);
 //   }

    //else {
        vec3 reflectDir = reflect(-L, N);
        spec = pow(max(dot(V, reflectDir), 0.0), reflectance * 8.0);
    //}

    vec3 specular = vec3(1.0) * spec; // assuming bright white light color
    return baseColor * diffuse;
}

void main() {
	// Discard if in alpha mask

	if (mat_hasAlphaMaskMap == 1) {
		if (0.5 > texture(mat_alphaMaskMap, TexCoords).r) {
			discard;
		}
	}

	// Color

	vec4 baseColor = mat_baseColor;
	if (mat_hasDiffuseMap == 1) {
		baseColor = texture(mat_diffuseMap, TexCoords);
		baseColor.rgb = sRGBToLinear(baseColor.rgb);
	}

	if (mat_hasDiffuseMap2 == 1) {
		vec4 baseColor2 = texture(mat_diffuseMap2, TexCoords);
		baseColor2.rgb = sRGBToLinear(baseColor2.rgb);

		baseColor = baseColor  *         Color.r
		          + baseColor2 * (1.0f - Color.r);
	}

	// Normal

	vec3 normal = Normal;
	if (mat_hasNormalMap == 1) {
		normal = texture(mat_normalMap, TexCoords).xyz;
	}

	if (mat_hasNormalMap2 == 1) {
		vec3 normal2 = texture(mat_normalMap2, TexCoords).xyz;

		normal = normal  *         Color.r
		       + normal2 * (1.0f - Color.r);
	}

	// Reflectance

	float reflectance = mat_reflectance;
	if (mat_hasReflectanceMap == 1) {
		reflectance = texture(mat_reflectanceMap, TexCoords).r;
	}

	// Ambient occlusion

	float ao = mat_ao;
	if (mat_hasAoMap == 1) {
		ao = texture(mat_aoMap, TexCoords).r;
	}

	vec3 N = normalize(normal);
	vec3 V = normalize(CameraPos - WorldPos);

	vec3 color = vec3(0);

//	for (int i = 0; i < lightCounts.x; i++) {
//		vec3 P = pointLights[i].Position.xyz;
//		vec3 L = P - WorldPos;
//
//		float R = pointLights[i].Position.w;
//
//		color += BRDF(N, V, L, baseColor.xyz, reflectance)
//		       * getDistanceAtt(L, 1 / pow(R, 2))
//			   * PointLightShadow(-L, R);
//	}
	
	for (int i = 0; i < lightCounts.y; i++) {
		vec3 L = directionalLights[i].InvDirection.xyz;

		//vec3 lightColor = directionalLights[i].Color.rgb;

		color += BRDF(N, V, L, baseColor.xyz, reflectance)
		       * DirectionalLightShadow(DirectionalLightPos[i]);
	}

	vec3 ambient  = max(ambiance - ao, 0) * baseColor.rgb;
	vec3 emissive = mat_emissive * baseColor.rgb;

	color += ambient + emissive;

	FragColor = vec4(linearToSRGB(color), baseColor.a);
}
