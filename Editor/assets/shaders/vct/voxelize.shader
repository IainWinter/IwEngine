#shader Vertex
#version 450

layout (location = 0) in vec3 vert;

uniform mat4 model;

void main() {
	gl_Position = model * vec4(vert, 1);
}

#shader Geometry
#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

out vec3 WorldPos;

void main() {
	const vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	const vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	const vec3 p = abs(cross(p1, p2));

	for (uint i = 0; i < 3; i++) {
		WorldPos = gl_in[i].gl_Position.xyz;

		if (   p.z > p.x
			&& p.z > p.y)
		{
			gl_Position = vec4(WorldPos.x, WorldPos.y, 0, 1);
		}

		else if (p.x > p.y
			  && p.x > p.z)
		{
			gl_Position = vec4(WorldPos.y, WorldPos.z, 0, 1);
		}

		else {
			gl_Position = vec4(WorldPos.x, WorldPos.z, 0, 1);
		}

		EmitVertex();
	}

	EndPrimitive();
}

#shader Fragment
#version 450

uniform vec4 baseColor;

layout(RGBA8) uniform image3D voxelTexture;

in vec3 WorldPos;

vec3 scaleAndBias(vec3 p)                { return 0.5f * p + vec3(0.5f); }
bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

void main() {
	if (!isInsideCube(WorldPos, 0)) return;

	vec3  voxel = scaleAndBias(WorldPos);
	ivec3 dim   = imageSize(voxelTexture);

	imageStore(voxelTexture, ivec3(dim * voxel), baseColor);
}
