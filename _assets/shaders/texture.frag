#version 330

uniform vec4 mat_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
uniform float mat_alphaThresh = 0.0f;

uniform float mat_hasTexture;
uniform sampler2D mat_texture;

in vec2 TexCoords;

out vec4 FragColor;

void main() {
	vec4 color = mat_color;

	if (mat_hasTexture == 1.0) {
		color *= texture(mat_texture, TexCoords);
	}

	if (color.a < mat_alphaThresh) {
		discard;
	}

	FragColor = color;
}