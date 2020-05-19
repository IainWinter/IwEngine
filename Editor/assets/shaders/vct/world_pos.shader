#shader Vertex
#version 430

#include shaders/camera.shader

layout (location = 0) in vec3 vert;

out vec3 WorldPos;

uniform mat4 model;

void main() {
	WorldPos = vec3(model * vec4(vert, 1));
	gl_Position = viewProj * vec4(WorldPos, 1);
}

#shader Fragment
#version 430

in vec3 WorldPos;

out vec4 FragColor;

void main() {
	FragColor = vec4(WorldPos, 1);
}
