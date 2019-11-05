#version 430 core

layout (std140, column_major) uniform Camera {
	mat4 proj;
	mat4 view;
};

uniform mat4 model;

layout (location = 0) in vec3 vert;
layout (location = 4) in vec2 uv;

out vec2 TexCoords;

void main() {
	TexCoords = uv;
	gl_Position =  proj * view * model * vec4(vert, 1);
}
