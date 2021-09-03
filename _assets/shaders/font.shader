#shader Vertex
#version 330

#include shaders/camera.shader

uniform mat4 model;

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 uv;

out vec2 UV;

void main() {
	UV = uv;
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment
#version 330

uniform vec4 mat_color;
uniform float mat_hasFontMap;
uniform sampler2D mat_fontMap;

uniform float width = 0.1f;
uniform float edge  = 0.2f;

in vec2 UV;

out vec4 PixelColor;

void main() {
	vec4 color = mat_color;

	if (mat_hasFontMap == 1) {
		float distance = 1.0f - texture2D(mat_fontMap, UV).a;
		float alpha = 1.0f - smoothstep(width, width + edge, distance);

		color.a *= alpha;
	}

	if (color.a < .2)
	{
		discard;
	}

	PixelColor = color;
}
