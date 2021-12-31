#version 330

in vec2 TexCoords;
in vec3 FragPos;

out vec4 FragColor;

#include shaders/noise.glsl

void main()
{
	vec3 co = vec3(FragPos);
	float w = 0.0;
	float scale = 0.8;
	float detail = 16.0;
	float distortion = 1.6;

	float value;
	vec4 color;

	node_noise_texture_2d(co, w, scale, detail, distortion, value, color);

	vec3 orange = vec3(1, .1, 0);
	FragColor = vec4(orange * value, 1.0);
}