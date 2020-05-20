#shader Vertex
#version 450

#include shaders/camera.shader

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;

out vec3 WorldPos;
out vec3 Normal;
out vec3 CameraPos;

uniform mat4 model;

void main() {
	vec4 worldPos = model * vec4(vert, 1);

	WorldPos  = worldPos.xyz;
	Normal    = normalize(transpose(inverse(mat3(model))) * normal);
	CameraPos = camPos.xyz;

	gl_Position = viewProj * worldPos;
}

#shader Fragment
#version 450

#define VOXEL_SIZE 1.0f / 64.0f

#include shaders/lights.shader
#include shaders/gamma_correction.shader

in vec3 WorldPos;
in vec3 Normal;
in vec3 CameraPos;

out vec4 PixelColor;

uniform vec4 mat_baseColor;
uniform float mat_reflectance;
uniform sampler3D mat_voxelMap;

// -------------------------------------------------------
//
//                    Helper functions
//
// -------------------------------------------------------

vec3 scaleAndBias(vec3 p)                { return 0.5f * p + vec3(0.5f); }
bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

// -------------------------------------------------------
//
//   Calculate soft shadows by tracing a ray (cone)
//    through the mipmap levels of the voxel map
//
// -------------------------------------------------------

// Returns a soft shadow blend by using shadow cone tracing.
// Uses 2 samples per step, so it's pretty expensive.
float TraceSoftShadow(
	vec3 from,
	vec3 direction,
	float targetDistance)
{
	from += Normal * 0.05f; // Removes artifacts but makes self shadowing for dense meshes meh.

	float acc = 0;
	float dist = 3 * VOXEL_SIZE;

	// I'm using a pretty big margin here since I use an emissive light ball with a pretty big radius in my demo scenes.
	float STOP = targetDistance - 16 * VOXEL_SIZE;

	while (dist < STOP
		&& acc < 1)
	{
		vec3 c = from + dist * direction;
		if (!isInsideCube(c, 0)) break;
		c = scaleAndBias(c);
		float l = pow(dist, 2); // Experimenting with inverse square falloff for shadows.
		float s1 = 0.062 * textureLod(mat_voxelMap, c, 1 + 0.75 * l).a;
		float s2 = 0.135 * textureLod(mat_voxelMap, c, 4.5      * l).a;
		float s = s1 + s2;
		acc += (1 - acc) * s;
		dist += 0.9 * VOXEL_SIZE * (1 + 0.05 * l);
	}

	return 1 - pow(smoothstep(0, 1, acc * 1.4), 1.0 / 1.4);
}

// -------------------------------------------------------
//
//   Calculate direct lighting with a simple phong brdf
//
// -------------------------------------------------------

vec3 BRDF_phong(
	vec3 N,
	vec3 V,
	vec3 L,
	vec3 baseColor,
	float reflectance,
	int blinn)
{
	vec3  nL    = normalize(L);
	float NdotL = clamp(dot(N, nL), 0.0f, 1.0f);

	float diff = max(NdotL, 0.0);
	float spec = 0.0;

	if(blinn == 1) {
		vec3 halfwayDir = normalize(L + V);  
		spec = pow(max(dot(N, halfwayDir), 0.0), reflectance * 8.0);
    }

	else {
		vec3 reflectDir = reflect(-L, N);
		spec = pow(max(dot(V, reflectDir), 0.0), reflectance * 8.0);
	}

	if (diff > 0) {
		diff = min(diff, TraceSoftShadow(WorldPos, nL, length(L)));
	}

	vec3 specular = vec3(1.0) * spec; // bright white light color. should pass light color
	return baseColor * diff;
}

void main() {
	// Material properties
	vec4  baseColor   = mat_baseColor;
	float reflectance = mat_reflectance;

	vec4 color = vec4(0, 0, 0, 1);

	vec3 N = normalize(Normal);
	vec3 V = normalize(CameraPos - WorldPos);

	for (int i = 0; i < pointLightCount; i++) {
		float R = pointLights[i].Radius;
		vec3 P  = pointLights[i].Position;
		vec3 L  = P - WorldPos;

		color.xyz = color.xyz
			+ BRDF_phong(N, V, L, baseColor.xyz, reflectance, 0)
			* DistanceAttenuation(L, R)
			/** PointLightShadow(-L, R)*/;
	}

	PixelColor = color;
}
