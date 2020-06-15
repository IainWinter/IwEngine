#version 450

#include shaders/lights.shader
#include shaders/gamma_correction.shader
#include shaders/vct/bounds.shader

in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoords;

// Material
uniform vec4  mat_baseColor;
uniform float mat_reflectance;
uniform float mat_emissive;

uniform float     mat_hasDiffuseMap;
uniform sampler2D mat_diffuseMap;

uniform float     mat_hasNormalMap;
uniform sampler2D mat_normalMap;

uniform float     mat_hasReflectanceMap;
uniform sampler2D mat_reflectanceMap;

uniform float     mat_hasShadowMap; // take out of material at some point
uniform sampler2D mat_shadowMap;    // take out of material at some point

#include shaders/shadows.frag

// Voxel
uniform vec3 voxelBoundsScale;
layout(RGBA8) uniform image3D voxelTexture;

// Globals
uniform float ambiance;

// Simple version of the phong BRDF, only calcs diffuse color

vec3 LightColor(
	vec3 N,          // Normal of fragment
	vec3 L,          // Direction from light  to fragment
	vec3 lightColor) // light color (rgb)
{
	return lightColor * max(clamp(dot(N, normalize(L)), 0.0f, 1.0f), 0.0);
}

void main() {
	if (!isInsideCube(WorldPos, voxelBoundsScale, 0)) return;

	// Color

	vec4 baseColor = mat_baseColor;
	if (mat_hasDiffuseMap == 1) {
		baseColor = texture(mat_diffuseMap, TexCoords);
	}

	baseColor.rgb = sRGBToLinear(baseColor.rgb);

	// Normal

	vec3 normal = /*TBN[2]*/Normal;
	if (mat_hasNormalMap == 1) {
		normal = /*TBN * */texture(mat_normalMap, TexCoords).xyz;
	}

	// Reflectance

	float reflectance = mat_reflectance;
	if (mat_hasReflectanceMap == 1) {
		reflectance = texture(mat_reflectanceMap, TexCoords).r;
	}

	vec3 color = vec3(ambiance);

	vec3 N = normalize(normal);
	
	//for (int i = 0; i < lightCounts.x; i++) {
	//	vec3 P = pointLights[i].Position.xyz;
	//	vec3 L = P - WorldPos;

	//	vec3  lightColor  = pointLights[i].Color.rgb;
	//	float lightRadius = pointLights[i].Position.w;

	//	color += LightColor(N, L, lightColor)
	//		   * DistanceAttenuation(L, lightRadius);
	//}

	for (int i = 0; i < lightCounts.y; i++) {
		vec3 L = directionalLights[i].InvDirection.xyz;

		vec3 lightColor = directionalLights[i].Color.rgb;

		color += LightColor(N, L, lightColor)
			   * DirectionalLightShadow(DirectionalLightPos[i]);
	}

	color *= baseColor.rgb * baseColor.a;
	color += baseColor.rgb * baseColor.a * mat_emissive;

	vec3  voxel = scaleAndBias(WorldPos);
	ivec3 dim   = imageSize(voxelTexture);

	vec4 voxelColor = vec4(linearToSRGB(color), baseColor.a);

	imageStore(voxelTexture, ivec3(dim * voxel), voxelColor);
}
