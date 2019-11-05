#version 430 core

layout (std140, column_major) uniform Camera {
	mat4 proj;
	mat4 view;
};

uniform mat4 model;
uniform mat4 lightSpace;

//uniform sampler2D heightMap;

in vec3 vert;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec2 uv;

out vec3 WorldPos;
out vec4 LightPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;

void main() {
	WorldPos  = (model * vec4(vert, 1)).xyz;
	LightPos =  lightSpace * vec4(WorldPos, 1);
	TexCoords = uv;

	mat3 modelVector = transpose(inverse(mat3(model)));

	vec3 T = normalize(modelVector * tangent);
	vec3 B = normalize(modelVector * bitangent);
	vec3 N = normalize(modelVector * normal);
	TBN = mat3(T, B, N);

	//float height = texture(heightMap, TexCoords).r;

	Normal = normalize(modelVector * normal);
	gl_Position =  proj * view * model * vec4(vert, 1);
}
