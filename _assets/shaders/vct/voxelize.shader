#shader Vertex
#version 450

#include shaders/shadows.geom

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;
layout(location = 4) in vec2 uv;

out vec3 gNormal;
out vec2 gTexCoords;

uniform mat4 model;

void main() {
	gNormal = normalize(transpose(inverse(mat3(model))) * normal);
	gTexCoords = uv;

	vec4 worldPos = model * vec4(vert, 1);

	SetDirectionalLightPos(worldPos);

	gl_Position = worldPos;
}

#shader Geometry shaders/vct/voxelize.geom
#shader Fragment shaders/vct/voxelize.frag
