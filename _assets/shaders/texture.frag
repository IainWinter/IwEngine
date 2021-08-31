#version 330

uniform vec4 mat_color;
uniform float mat_alphaThresh = 0.0;

uniform float mat_hasTexture;
uniform sampler2D mat_texture;

layout(location = 0) in vec2 TexCoords;

out vec4 FragColor;

void main() {
	vec4 color = vec4(1.0f);

	if (mat_hasTexture == 1.0) {
		color = texture(mat_texture, TexCoords);
	}

	color *= mat_color;

	if (color.a < mat_alphaThresh) {
		discard;
	}

	FragColor = color;
}