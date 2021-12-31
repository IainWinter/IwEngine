#shader Vertex
#version 420

#include shaders/camera.shader
#include shaders/shadows.vert

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent;
layout (location = 4) in vec2 uv;

out vec3 WorldPos;
out vec3 CameraPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;

uniform mat4 model;

// material parameters

uniform float     mat_hasDisplacementMap;
uniform sampler2D mat_displacementMap;

void main() {
	vec3 displacement = vec3(0);
	if (mat_hasDisplacementMap == 1) {
		displacement = normal * texture(mat_displacementMap, uv).r;
	}

	vec4 worldPos = model * vec4(vert + displacement, 1);

	WorldPos  = worldPos.xyz;
	CameraPos = camPos.xyz;
	TexCoords = uv;

	mat3 modelVector = transpose(inverse(mat3(model)));

	vec3 T = normalize(modelVector * tangent);
	vec3 B = normalize(modelVector * bitangent);
	vec3 N = normalize(modelVector * normal);

	TBN = mat3(T, B, N);
	Normal = normalize(modelVector * normal);

	SetDirectionalLightPos(worldPos);

	gl_Position = viewProj * worldPos;
}

#shader Fragment shaders/test/pbr_noise.frag
