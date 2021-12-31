#shader Vertex
#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 uv;

#include shaders/camera.shader

uniform mat4 model;

out vec2 TexCoords;
out vec3 FragPos;

void main() {
	vec4 worldPos = model * vec4(vert, 1.f);

	TexCoords = uv;
	FragPos = worldPos.xyz;
	gl_Position = viewProj * worldPos;
}

#shader Fragment shaders/noise_test.frag
