#version 450

#define MAX_DIRECTIONAL_LIGHTS 4

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 gNormal[];
in vec2 gTexCoords[];
in vec4 gDirectionalLightPos[][MAX_DIRECTIONAL_LIGHTS];

out vec3 WorldPos;
out vec3 Normal;
out vec2 TexCoords;
out vec4 DirectionalLightPos[MAX_DIRECTIONAL_LIGHTS];

uniform vec3 voxelBoundsScaleInv;

void main() {
	vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 p = abs(cross(p1, p2));

	for (int i = 0; i < 3; i++) {
		WorldPos = gl_in[i].gl_Position.xyz * voxelBoundsScaleInv;
		Normal = gNormal[i];
		TexCoords = gTexCoords[i];
		DirectionalLightPos = gDirectionalLightPos[i];

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
