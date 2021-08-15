#shader Vertex
#version 330

#include shaders/camera.shader

uniform mat4 model;

layout (location = 0) in vec3 vert;
layout (location = 1) in vec2 uv;

void main() {
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment
#version 330

uniform vec4 mat_color = vec4(1, 0, 0, 1);

out vec4 FragColor;

void main() {
	FragColor = mat_color;
}
