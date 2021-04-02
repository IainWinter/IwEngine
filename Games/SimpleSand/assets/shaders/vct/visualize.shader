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

in vec2 TexCoords;
in vec3 CameraPos;

out vec4 PixelColor;

uniform sampler2D mat_back;
uniform sampler2D mat_front;
uniform sampler3D mat_world;
uniform int mat_level;

uniform vec3  voxelBoundsScale;
uniform vec3  voxelBoundsScaleInv;
uniform float voxelSize;

void main() {
	float level = float(mat_level);

	vec3 origin = isInsideCube(CameraPos, voxelBoundsScale, 0.2f) 
		? CameraPos
		: texture(mat_front, TexCoords).xyz;

	vec3 direction = texture(mat_back, TexCoords).xyz - origin;
	
	float maxDistance = max(5, length(direction));
	float distance    = 0;

	direction = normalize(direction);

	vec4 color = vec4(0.0f);

	while (distance <= maxDistance
		&& color.w  < 1.0f)
	{
		vec3 position = origin + direction * distance;

		if (!isInsideCube(position, voxelBoundsScale, 0)) { // Todo: Check if this is worth it
			break;
		}

		position = scaleAndBias(position * voxelBoundsScaleInv);

		vec4  sample_ = textureLod(mat_world, position, level);
		float weight = 1.0f - color.w;

		color += sample_ * weight;
		distance += voxelSize * 0.01f;
	}

	PixelColor = color;
}
