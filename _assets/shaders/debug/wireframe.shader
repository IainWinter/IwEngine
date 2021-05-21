#shader Vertex
#version 330

#include shaders/camera.shader

uniform mat4 model;

layout (location = 0) in vec3 vert;

void main() {
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment
#version 330

out vec4 FragColor;

uniform vec4 mat_color;

void main() {
	FragColor = mat_color;
}
