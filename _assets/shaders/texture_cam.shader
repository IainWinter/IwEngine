#shader Vertex
#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 uv;

#include shaders/camera.shader

uniform mat4 model;

out vec2 TexCoords;

void main() {
	TexCoords = uv;
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment shaders/texture.frag
