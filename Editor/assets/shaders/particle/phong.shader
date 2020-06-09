#shader Vertex
#version 330

#include shaders/shadows.vert

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in mat4 i_model;
layout(location = 4) in mat4 i_modelViewProj;

out vec3 WorldPos;
out vec3 Normal;
out vec2 TexCoords;

void main() {
	vec4 worldPos = i_model * vec4(vert, 1);

	WorldPos  = worldPos.xyz;
	Normal    = normal;
	TexCoords = uv;

	SetDirectionalLightPos(worldPos);

	gl_Position = i_modelViewProj * vec4(vert, 1);
}

#shader Fragment
#version 330

in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

// Material

uniform vec4  mat_baseColor;
uniform float mat_reflectance;

uniform float     mat_hasDiffuseMap;
uniform sampler2D mat_diffuseMap;

uniform float     mat_hasNormalMap;
uniform sampler2D mat_normalMap;

uniform float     mat_hasShadowMap;       // take out of material at some point
uniform sampler2D mat_shadowMap;          // take out of material at some point

// Settings
uniform int BLINN = 1;

#include shaders/lights.shader
#include shaders/camera.shader
#include shaders/shadows.frag
#include shaders/gamma_correction.shader

// Phong

vec3 BRDF(
	vec3 N,
	vec3 V,
	vec3 L,
	vec4 baseColor,
	float reflectance)
{
	vec3  nL = normalize(L);
	float NdotL = clamp(dot(N, nL), 0.0f, 1.0f);

	// Diffuse

	float diff = max(NdotL, 0.0f);

	// Specular

	float spec = 0.0f;

	if (BLINN == 1) spec = dot(N, normalize(L + V));
	else            spec = dot(V, reflect(-L, N));

	spec = max(pow(spec, reflectance * 8.0f), 0.0f);

	// Mix color

	diff = diff * baseColor.a;

	return vec3(diff + spec);
}

void main() {
	// Color
	
	vec4 baseColor = mat_baseColor;
	if (mat_hasDiffuseMap == 1) {
		baseColor = texture(mat_diffuseMap, TexCoords);
		baseColor.rgb = sRGBToLinear(baseColor.rgb);
	}

	//if (baseColor.a < 0.5f) {
	//	discard;
	//}

	// Normal

	vec3 normal = Normal;
	if (mat_hasNormalMap == 1) {
		normal = texture(mat_normalMap, TexCoords).xyz;
	}

	// Reflectance

	float reflectance = mat_reflectance;
	/*if (mat_hasReflectanceMap == 1) {
		reflectance = texture(mat_reflectanceMap, TexCoords).r;
	}*/

	vec3 N = normalize(normal);
	vec3 V = normalize(camPos.xyz - WorldPos);

	vec3 color = vec3(0.0f);

	for (int i = 0; i < directionalLightCount; i++) {
		vec3 L = directionalLights[i].InvDirection;

		color += BRDF(N, V, L, baseColor, reflectance)
			   * DirectionalLightShadow(DirectionalLightPos[i]);
	}

	FragColor = vec4(baseColor.rgb, 1);//vec4(color, baseColor.a);
}
