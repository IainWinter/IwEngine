#shader Vertex
#version 330

#include shaders/camera.shader

layout(location = 0) in vec2 vert;
layout(location = 1) in vec2 uv;

uniform mat4 model;

out vec2 TexCoords;

void main() {
	TexCoords = uv;
	gl_Position = viewProj * model * vec4(vert, 0, 1);
}

#shader Fragment
#version 330

uniform vec4 mat_color;
uniform float mat_hasTexture;
uniform sampler2D mat_texture;

in vec2 TexCoords;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out float IsStatic;

void main() {
	vec4 color = mat_color;
	if (mat_hasTexture == 1.0) {
		color = texture(mat_texture, TexCoords);
	}

	FragColor = color;
	IsStatic  = color.a;
}