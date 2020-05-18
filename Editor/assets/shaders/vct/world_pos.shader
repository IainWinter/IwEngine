#shader Vertex
#version 430

#include shaders/camera.shader

layout (location = 0) in vec3 vert;

out vec3 WorldPos;

uniform mat4 model;

void main() {
	vec4 worldPos = model * vec4(vert, 1);
	WorldPos = worldPos.xyz;
	gl_Position = viewProj * worldPos;
}

#shader Fragment
#version 430

in vec3 WorldPos;

out vec4 FragColor;

void main() {
	FragColor = vec4(WorldPos, 1);
}
