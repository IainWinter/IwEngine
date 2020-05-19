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

#define STEP_LENGTH 0.005f
#define INV_STEP_LENGTH 1.0f / STEP_LENGTH

in vec2 TexCoords;
in vec3 CameraPos;

out vec4 PixelColor;

uniform sampler2D mat_back;
uniform sampler2D mat_front;
uniform sampler3D mat_world;
uniform float mat_level;

vec3 scaleAndBias(vec3 p)          { return 0.5f * p + vec3(0.5f); }
bool isInsideCube(vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

void main() {
	vec3 cameraPosition        = CameraPos;
	vec2 textureCoordinateFrag = TexCoords;

	const float mipmapLevel = 0;

	// Initialize ray.
	const vec3 origin = isInsideCube(cameraPosition, 0.2f)
		? cameraPosition
		: texture(mat_front, textureCoordinateFrag).xyz;

	vec3 direction = texture(mat_back, textureCoordinateFrag).xyz - origin;
	const uint numberOfSteps = uint(INV_STEP_LENGTH * length(direction));
	direction = normalize(direction);

	// Trace.
	vec4 color = vec4(0.0f);
	for (uint step = 0; step < numberOfSteps && color.a < 0.99f; ++step) {
		const vec3 currentPoint = origin + STEP_LENGTH * step * direction;
		vec4 currentSample = textureLod(mat_world, scaleAndBias(currentPoint), mipmapLevel);
		color += (1.0f - color.a) * currentSample;
	}

	color.rgb = pow(color.rgb, vec3(1.0 / 2.2));

	PixelColor = color;
}
