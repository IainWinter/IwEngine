#shader Vertex
#version 450

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;
layout(location = 4) in vec2 uv;

out vec3 gNormal;
out vec2 gTexCoords;

uniform mat4 model;

void main() {
	gNormal = normalize(transpose(inverse(mat3(model))) * normal);
	gTexCoords = uv;
	gl_Position = model * vec4(vert, 1);
}

#shader Geometry
#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 gNormal[];
in vec2 gTexCoords[];

out vec3 WorldPos;
out vec3 Normal;
out vec2 TexCoords;

uniform vec3 voxelBoundsScaleInv;

void main() {
	vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 p = abs(cross(p1, p2));

	for (int i = 0; i < 3; i++) {
		WorldPos = gl_in[i].gl_Position.xyz * voxelBoundsScaleInv;
		Normal = gNormal[i];
		TexCoords = gTexCoords[i];

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

// Material
uniform vec4 mat_baseColor;
uniform float mat_reflectance;
uniform float mat_emissive;

uniform float mat_hasDiffuseMap;
uniform sampler2D mat_diffuseMap;

uniform float mat_hasNormalMap;
uniform sampler2D mat_normalMap;

uniform float mat_hasReflectanceMap;
uniform sampler2D mat_reflectanceMap;

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
	
	for (int i = 0; i < pointLightCount; i++) {
		vec3 P = pointLights[i].Position;
		vec3 L = P - WorldPos;

		float lightRadius = pointLights[i].Radius;
		vec3  lightColor  = vec3(1.0f);//pointLights[i].Color;

		color += LightColor(N, L, lightColor)
			   * DistanceAttenuation(L, lightRadius);
	}

	for (int i = 0; i < directionalLightCount; i++) {
		vec3 L = directionalLights[0].InvDirection;

		color += LightColor(N, L, vec3(1.0f));
	}

	color *= baseColor.rgb * baseColor.a;
	color += baseColor.rgb * mat_emissive;

	vec3  voxel = scaleAndBias(WorldPos);
	ivec3 dim   = imageSize(voxelTexture);

	vec4 voxelColor = vec4(linearToSRGB(color), baseColor.a);

	imageStore(voxelTexture, ivec3(dim * voxel), voxelColor);
}
