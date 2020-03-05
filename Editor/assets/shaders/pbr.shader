#shader Vertex
#version 330

layout(std140, column_major) uniform Camera{
	mat4 viewProj;
	vec4 camPos;
};

uniform mat4 model;
uniform mat4 lightSpace;

// material parameters

uniform float mat_hasDisplacementMap;
uniform sampler2D mat_displacementMap;

in vec3 vert;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec2 uv;

out vec3 WorldPos;
out vec4 LightPos;
out vec3 CameraPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;

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