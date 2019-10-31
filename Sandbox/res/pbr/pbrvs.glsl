#version 430 core

layout (std140, column_major) uniform Camera {
	mat4 proj;
	mat4 view;
};

uniform mat4 model;

in vec3 vert;
in vec3 normal;

out vec3 Normal;
out vec3 WorldPos;

void main() {
	Normal   = normal;
	WorldPos = (model * vec4(vert, 1)).xyz;
	gl_Position =  proj * view * model * vec4(vert, 1);
}
