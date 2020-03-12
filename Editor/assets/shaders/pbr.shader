#shader Vertex
#version 330

layout(std140, column_major) uniform Camera {
	mat4 viewProj;
	vec4 camPos;
};

uniform mat4 model;
uniform mat4 lightSpace;

// material parameters

uniform float mat_hasDisplacementMap;
uniform sampler2D mat_displacementMap;

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 tangent;
layout (location = 3) in vec3 bitangent;
layout (location = 4) in vec2 uv;

layout (location = 0) out vec3 WorldPos;
layout (location = 1) out vec4 LightPos;
layout (location = 2) out vec3 CameraPos;
layout (location = 3) out vec2 TexCoords;
layout (location = 4) out vec3 Normal;
layout (location = 5) out mat3 TBN;

void main() {
	vec3 displacement = vec3(0);
	if (mat_hasDisplacementMap == 1) {
		displacement = normal * texture(mat_displacementMap, uv).r;
	}

	vec4 worldPos = model * vec4(vert + displacement, 1);

	WorldPos  = worldPos.xyz;
	LightPos  = lightSpace * vec4(WorldPos, 1);
	CameraPos = camPos.xyz;
	TexCoords = uv;

	mat3 modelVector = transpose(inverse(mat3(model)));

	vec3 T = normalize(modelVector * tangent);
	vec3 B = normalize(modelVector * bitangent);
	vec3 N = normalize(modelVector * normal);

	TBN = mat3(T, B, N);
	Normal = normalize(modelVector * normal);

	gl_Position = viewProj * worldPos;
}

#shader Fragment shaders/pbr.frag
