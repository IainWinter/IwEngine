#shader Vertex
#version 450

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;

out vec3 gNormal;

uniform mat4 model;

void main() {
	gNormal = normalize(transpose(inverse(mat3(model))) * normal);
	gl_Position = model * vec4(vert, 1);
}

#shader Geometry
#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 gNormal[];

out vec3 WorldPos;
out vec3 Normal;

uniform vec3 voxelBoundsScaleInv;

void main() {
	vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	vec3 p = abs(cross(p1, p2));

	for (int i = 0; i < 3; i++) {
		WorldPos = gl_in[i].gl_Position.xyz * voxelBoundsScaleInv;
		Normal = gNormal[i];

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

#include shaders/lights.shader
#include shaders/vct/bounds.shader

in vec3 WorldPos;
in vec3 Normal;

// material properties. No 'mat_' because this is technically a shadow cast
uniform vec4  baseColor;
uniform float reflectance;
uniform float ambiance;

uniform vec3 voxelBoundsScale;

layout(RGBA8) uniform image3D voxelTexture;

// Simple version of the phong BRDF, only calcs diffuse color

vec3 LightColor(
	vec3 N,          // Normal of fragment
	vec3 L,          // Direction from light  to fragment
	vec3 lightColor) // light color (rgb)
{
	return lightColor * max(clamp(dot(N, normalize(L)), 0.0f, 1.0f), 0.0);
}

void main() {
	if (!isInsideCube(WorldPos, voxelBoundsScale, 0)) return;

	vec4 color = vec4(0, 0, 0, baseColor.a);

	vec3 N = normalize(Normal);
	
	for (int i = 0; i < pointLightCount; i++) {
		float R = pointLights[i].Radius;
		vec3  P = pointLights[i].Position;
		vec3  L = P - WorldPos;
	
		color.xyz += LightColor(N, L, vec3(1)) * DistanceAttenuation(L, R);
	}

	color.xyz *= baseColor.xyz;
	color.xyz += baseColor.xyz * vec3(ambiance);

	vec3  voxel = scaleAndBias(WorldPos);
	ivec3 dim   = imageSize(voxelTexture);

	imageStore(voxelTexture, ivec3(dim * voxel), color);
}
