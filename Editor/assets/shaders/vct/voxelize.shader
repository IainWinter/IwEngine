#shader Vertex
#version 450

layout(location = 0) in vec3 vert;
//layout(location = 1) in vec3 normal;

//out vec3 gNormal;

uniform mat4 model;

void main() {
	//gNormal = normalize(transpose(inverse(mat3(model))) * normal);
	gl_Position = model * vec4(vert, 1);
}

#shader Geometry
#version 450

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

//in vec3 gNormal[];

out vec3 WorldPos;
//out vec3 Normal;

void main() {
	const vec3 p1 = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	const vec3 p2 = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
	const vec3 p = abs(cross(p1, p2));

	for (uint i = 0; i < 3; i++) {
		WorldPos = gl_in[i].gl_Position.xyz;
		//Normal = gNormal[i];

		if (p.z > p.x
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

in vec3 WorldPos;
//in vec3 Normal;

// material properties. No 'mat_' because this is technically a shadow cast
uniform vec4  baseColor;
uniform float reflectance;

layout(RGBA8) uniform image3D voxelTexture;
uniform vec3 CameraPos;

vec3 scaleAndBias(vec3 p) { return 0.5f * p + vec3(0.5f); }
bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

vec3 BRDF_phong(
	vec3 N,            // Normal of fragment
	vec3 V,            // Direction from camera to fragment
	vec3 L,            // Direction from light  to fragment
	vec3 baseColor,    // base color  of fragment (rgb)
	float reflectance, // reflectance of fragment 
	int blinn)         // blinn-phong or normal phong (1 or !1)
{
	vec3  nL = normalize(L);
	float NdotL = clamp(dot(N, nL), 0.0f, 1.0f);

	float diff = max(NdotL, 0.0);
	float spec = 0.0;

	if (blinn == 1) {
		vec3 halfwayDir = normalize(L + V);
		spec = pow(max(dot(N, halfwayDir), 0.0), reflectance * 8.0);
	}

	else {
		vec3 reflectDir = reflect(-L, N);
		spec = pow(max(dot(V, reflectDir), 0.0), reflectance * 8.0);
	}

	vec3 specular = vec3(1.0) * spec; // bright white light color. should pass light color
	return baseColor * diff + specular;
}

void main() {
	if (!isInsideCube(WorldPos, 0)) return;

	vec4 color = vec4(0);

	//vec3 N = normalize(Normal);
	//vec3 V = normalize(CameraPos - WorldPos);

	//for (int i = 0; i < pointLightCount; i++) {
	//	float R = pointLights[i].Radius;
	//	vec3 P  = pointLights[i].Position;
	//	vec3 L  = P - WorldPos;

	//	color.xyz += BRDF_phong(N, V, L, baseColor.xyz, reflectance, 0) * DistanceAttenuation(L, R);
	//}

	vec3  voxel = scaleAndBias(WorldPos);
	ivec3 dim = imageSize(voxelTexture);

	imageStore(voxelTexture, ivec3(dim * voxel), color);
}
