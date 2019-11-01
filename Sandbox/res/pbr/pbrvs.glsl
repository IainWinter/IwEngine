#version 430 core

layout (std140, column_major) uniform Camera {
	mat4 proj;
	mat4 view;
};

uniform mat4 model;

in vec3 vert;
in vec3 normal;
in vec2 uv;

out vec3 Normal;
out vec3 WorldPos;
out vec2 TexCoords;

void main() {
	Normal    = normal;
	WorldPos  = (model * vec4(vert, 1)).xyz;
	TexCoords = uv;
	gl_Position =  proj * view * model * vec4(vert, 1);
}
