#shader Vertex
#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in mat4 i_model;

#include shaders/camera.shader

uniform mat4 model;

out vec2 FragPos;

void main() {
	gl_Position = viewProj * model * i_model * vec4(vert, 1);
}

#shader Fragment
#version 330

uniform vec4 mat_color;

out vec4 FragColor;

void main() {
	FragColor = mat_color;
}
