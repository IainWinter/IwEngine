#shader Vertex
#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 uv;

#include shaders/camera.shader

uniform mat4 model;

out vec2 FragPos;

void main() {
	FragPos = uv;
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment
#version 330

in vec2 FragPos;

uniform sampler2D mat_texture;
uniform float mat_alphaThresh;
uniform vec4 mat_color;

out vec4 FragColor;

void main() {
	vec4 color = mat_color * texture(mat_texture, FragPos);
	if (color.a < mat_alphaThresh) {
		discard;
	}

	FragColor = color;
}
