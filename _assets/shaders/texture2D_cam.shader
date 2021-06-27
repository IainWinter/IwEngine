#shader Vertex
#version 330

#include shaders/camera.shader

layout(location = 0) in vec2 vert;

uniform mat4 model;

out vec2 FragPos;

void main() {
	FragPos = vert / vec2(2.0f) + vec2(0.5f);
	gl_Position = viewProj * model * vec4(vert, 0, 1);
}

#shader Fragment
#version 330

uniform vec4 mat_color;
uniform float mat_hasTexture;
uniform sampler2D mat_texture;

in vec2 TexCoords;

out vec4 FragColor;

void main() {
	vec4 color = mat_color;
	if (mat_hasTexture == 1.0) {
		color = texture(mat_texture, TexCoords);
	}

	if (color.a < 0.5) {
		discard;
	}

	FragColor = color;
}