#shader Vertex
#version 450

#define MAX_DIRECTIONAL_LIGHTS 4

layout(std140, column_major) uniform Shadows {
	int shadows_pad1, shadows_pad2, shadows_pad3;

	int directionalLightSpaceCount;
	mat4 directionalLightSpaces[MAX_DIRECTIONAL_LIGHTS];
};

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;
layout(location = 4) in vec2 uv;

out vec3 gNormal;
out vec2 gTexCoords;
out vec4 gDirectionalLightPos[MAX_DIRECTIONAL_LIGHTS];

uniform mat4 model;

void main() {
	gNormal = normalize(transpose(inverse(mat3(model))) * normal);
	gTexCoords = uv;

	vec4 worldPos = model * vec4(vert, 1);

	for (int i = 0; i < directionalLightSpaceCount; i++) {
		gDirectionalLightPos[i] = directionalLightSpaces[i] * worldPos;
	}

	gl_Position = worldPos;
}

#shader Geometry
#version 450

#define MAX_DIRECTIONAL_LIGHTS 4

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 gNormal[];
in vec2 gTexCoords[];
in vec4 gDirectionalLightPos[][MAX_DIRECTIONAL_LIGHTS];

out vec3 WorldPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 DirectionalLightPos[MAX_DIRECTIONAL_LIGHTS];

uniform vec3 voxelBoundsScaleInv;

void main() {
	vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 p = abs(cross(p1, p2));

	for (int i = 0; i < 3; i++) {
		WorldPos = gl_in[i].gl_Position.xyz * voxelBoundsScaleInv;
		Normal = gNormal[i];
		TexCoords = gTexCoords[i];
		DirectionalLightPos = gDirectionalLightPos[i];

		if (   p.z > p.x
			&& p.z > p.y)
		{
			gl_Position = vec4(WorldPos.x, WorldPos.y, 0, 1);
		}

		else if (p.x > p.y
			  && p.x > p.z)
		{
			gl_Position = vec4(WorldPos.y, WorldPos.z, 0, 1);
		}

		else {
			gl_Position = vec4(WorldPos.x, WorldPos.z, 0, 1);
		}

		EmitVertex();
	}

	EndPrimitive();
}

#shader Fragment
#version 450

#include shaders/lights.shader
#include shaders/gamma_correction.shader
#include shaders/vct/bounds.shader

in vec3 WorldPos;
in vec3 Normal;
in vec2 TexCoords;
in vec4 DirectionalLightPos[MAX_DIRECTIONAL_LIGHTS];

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

// Voxel
uniform vec3 voxelBoundsScale;
layout(RGBA8) uniform image3D voxelTexture;

// Globals
uniform float ambiance;

// Math functions

float linstep(
	float l,
	float h,
	float v)
{
	return clamp((v - l) / (h - l), 0.0, 1.0);
}

// Simple version of the phong BRDF, only calcs diffuse color

vec3 LightColor(
	vec3 N,          // Normal of fragment
	vec3 L,          // Direction from light  to fragment
	vec3 lightColor) // light color (rgb)
{
	return lightColor * max(clamp(dot(N, normalize(L)), 0.0f, 1.0f), 0.0);
}

// Shadows

float DirectionalLightShadow(
	vec4 coords4)
{
	if (mat_hasShadowMap == 0) {
		return 1.0f;
	}

	vec3 coords = (coords4.xyz / coords4.w) * 0.5 + 0.5;
	vec2 moments = texture(mat_shadowMap, coords.xy).rg;
	float compare = coords.z;

	if (compare > 1.0) { // test if like not having this brantch is faster?
		return 1.0;
	}

	float p = step(compare, moments.x);
	float v = max(moments.y - moments.x * moments.x, 0.00002);

	float d = compare - moments.x;
	float pMax = linstep(0.2, 1.0, v / (v + d * d));

	return min(max(p, pMax), 1.0);
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
	
	for (int i = 0; i < pointLightCount; i++) {
		vec3 P = pointLights[i].Position;
		vec3 L = P - WorldPos;

		float lightRadius = pointLights[i].Radius;
		vec3  lightColor  = vec3(1.0f);//pointLights[i].Color;

		color += LightColor(N, L, lightColor)
			   * DistanceAttenuation(L, lightRadius);
	}

	for (int i = 0; i < directionalLightCount; i++) {
		vec3 L = directionalLights[i].InvDirection;

		color += LightColor(N, L, vec3(1.0f))
			   * DirectionalLightShadow(DirectionalLightPos[i]);
	}

	color *= baseColor.rgb * baseColor.a;
	color += baseColor.rgb * baseColor.a * mat_emissive;

	vec3  voxel = scaleAndBias(WorldPos);
	ivec3 dim   = imageSize(voxelTexture);

	vec4 voxelColor = vec4(linearToSRGB(color), baseColor.a);

	imageStore(voxelTexture, ivec3(dim * voxel), voxelColor);
}
