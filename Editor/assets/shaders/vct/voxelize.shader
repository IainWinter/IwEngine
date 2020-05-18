#shader Vertex
#version 430

#include shaders/camera.shader

layout (location = 0) in vec3 vert;
layout (location = 1) in vec3 normal;

out vec3 WorldPos;
out vec3 Normal;

uniform mat4 model;

void main() {
	vec4 worldPos    = model * vec4(vert, 1);
	mat3 modelVector = transpose(inverse(mat3(model)));

	WorldPos  = worldPos.xyz;
	Normal    = normalize(modelVector * normal);

	gl_Position = viewProj * worldPos;
}

#shader Geometry
#version 430

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 WorldPos[];
in vec3 Normal[];

out vec3 worldPositionFrag;
out vec3 normalFrag;

// Dominant axis selection & Triangle projection

void main() {
	/*float x = abs(dot(Normal[i], vec3(1, 0, 0)));
	float y = abs(dot(Normal[i], vec3(0, 1, 0)));
	float z = abs(dot(Normal[i], vec3(0, 0, 1)));*/

	const vec3 p1 = WorldPos[1] - WorldPos[0]; // normal calculation?? but we already have normals
	const vec3 p2 = WorldPos[2] - WorldPos[0];
	const vec3 p = abs(cross(p1, p2));         // why abs

	for (uint i = 0; i < 3; i++) {
		worldPositionFrag = WorldPos[i];
		normalFrag        = Normal[i];

		if (   p.z > p.x
			&& p.z > p.y)
		{
			gl_Position = vec4(worldPositionFrag.x, worldPositionFrag.y, 0, 1);
		}

		else if (p.x > p.y
			  && p.x > p.z)
		{
			gl_Position = vec4(worldPositionFrag.y, worldPositionFrag.z, 0, 1);
		}

		else {
			gl_Position = vec4(worldPositionFrag.x, worldPositionFrag.z, 0, 1);
		}

		EmitVertex();
	}

	EndPrimitive();
}

#shader Fragment
#version 430

layout(RGBA8) uniform image3D voxelTexture;

in vec3 worldPositionFrag;
in vec3 normalFrag;

vec3 scaleAndBias(vec3 p)                { return 0.5f * p + vec3(0.5f); }
bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

void main() {
	vec3 color = vec3(normalFrag);
	//if (!isInsideCube(worldPositionFrag, 0)) return;

	//// Calculate diffuse lighting fragment contribution.
	//const uint maxLights = min(numberOfLights, MAX_LIGHTS);
	//for (uint i = 0; i < maxLights; ++i) color += calculatePointLight(pointLights[i]);
	//vec3 spec = material.specularReflectivity * material.specularColor;
	//vec3 diff = material.diffuseReflectivity * material.diffuseColor;
	//color = (diff + spec) * color + clamp(material.emissivity, 0, 1) * material.diffuseColor;

	// Output lighting to 3D texture.
	vec3 voxel = scaleAndBias(worldPositionFrag);
	ivec3 dim = imageSize(voxelTexture);
	
	float alpha = 1; // pow(1 - material.transparency, 4); // For soft shadows to work better with transparent materials.
	vec4 res = alpha * vec4(vec3(color), 1);

	imageStore(voxelTexture, ivec3(dim * voxel), res);
}
