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

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 4) in vec2 uv;

out vec3 WorldPos;
out vec3 CameraPos;
out vec2 TexCoords;
out vec3 Normal;
out vec4 DirectionalLightPos[MAX_DIRECTIONAL_LIGHTS];

uniform mat4 model;

// material parameters

uniform float     mat_hasDisplacementMap;
uniform sampler2D mat_displacementMap;

void main() {
	vec3 displacement = vec3(0);
	if (mat_hasDisplacementMap == 1) {
		displacement = normal * texture(mat_displacementMap, uv).r;
	}

	vec4 worldPos    = model * vec4(vert + displacement, 1);
	mat3 modelVector = transpose(inverse(mat3(model)));

	WorldPos  = worldPos.xyz;
	CameraPos = camPos.xyz;
	TexCoords = uv;
	Normal    = normalize(modelVector * normal);

	for (int i = 0; i < directionalLightSpaceCount; i++) {
		DirectionalLightPos[i] = directionalLightSpaces[i] * worldPos;
	}

	gl_Position = viewProj * worldPos;
}

#shader Fragment shaders/phong.frag