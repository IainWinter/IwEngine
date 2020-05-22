#shader Vertex
#version 450

#include shaders/camera.shader

layout (location = 0) in vec3 vert;
layout (location = 1) in vec2 uv;

out vec2 TexCoords;
out vec3 CameraPos;

void main() {
	TexCoords = uv;
	CameraPos = camPos.xyz;

	gl_Position = vec4(vert, 1);
}

#shader Fragment
#version 450

#include shaders/gamma_correction.shader
#include shaders/vct/bounds.shader

#define STEP_LENGTH 0.005f
#define INV_STEP_LENGTH 1.0f / STEP_LENGTH

in vec2 TexCoords;
in vec3 CameraPos;

out vec4 PixelColor;

uniform sampler2D mat_back;
uniform sampler2D mat_front;
uniform sampler3D mat_world;
uniform int mat_level;

void main() {
	float level = float(mat_level);

	vec3 origin = isInsideCube(CameraPos, 0.2f) ? CameraPos
												: texture(mat_front, TexCoords).xyz;

	vec3 direction = texture(mat_back, TexCoords).xyz - origin;
	float length = length(direction);
	
	direction = normalize(direction);

	int steps = int(INV_STEP_LENGTH * length);

	vec4 color = vec4(0.0f);
	for (int step = 0; step < steps && color.a <= 1.0f; step++) {
		vec3 p = origin + STEP_LENGTH * step * direction;
		vec4 s = textureLod(mat_world, scaleAndBias(p), level);

		color += (1.0f - color.a) * s;
	}

	color.rgb = linearToSRGB(color.rgb);

	PixelColor = color;
}
