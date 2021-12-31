#shader Vertex
#version 330

#include shaders/camera.shader

uniform mat4 model;

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 vert_color;

out vec3 Color;

void main() {
	Color = vert_color;
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment
#version 330

in vec3 Color;

out vec4 FragColor;

void main() {
	FragColor = vec4(Color, 1);
}
