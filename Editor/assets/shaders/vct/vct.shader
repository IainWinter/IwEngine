#shader Vertex
#version 450

#include shaders/camera.shader

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;

out vec3 WorldPos;
out vec3 Normal;
out vec3 Tangent;
out vec3 BiTangent;
out vec3 CameraPos;

uniform mat4 model;

void main() {
	vec4 worldPos = model * vec4(vert, 1);
	mat3 worldNom = transpose(inverse(mat3(model)));

	WorldPos  = worldPos.xyz;
	Normal    = normalize(worldNom * normal);
	Tangent   = normalize(worldNom * tangent);
	BiTangent = normalize(worldNom * bitangent);
	CameraPos = camPos.xyz;

	gl_Position = viewProj * worldPos;
}

#shader Fragment
#version 450

#define ISQRT2 0.707106

#include shaders/lights.shader
#include shaders/gamma_correction.shader

in vec3 WorldPos;
in vec3 Normal;
in vec3 Tangent;
in vec3 BiTangent;
in vec3 CameraPos;

out vec4 PixelColor;

uniform vec4 mat_baseColor;
uniform float mat_reflectance;
uniform sampler3D mat_voxelMap;

uniform float voxelSize    = 1 / 32.0f;
uniform float voxelSizeInv = 32.0f;

// -------------------------------------------------------
//
//                    Helper functions
//
// -------------------------------------------------------

vec3 scaleAndBias(vec3 p)                { return 0.5f * p + vec3(0.5f); }
bool isInsideCube(const vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

vec3 orthogonal(
	vec3 u)
{
	vec3 v = vec3(0.99146, 0.11664, 0.05832); // Any normalized vector.
	     u = normalize(u);
	return abs(dot(u, v)) > 0.99999f
		? cross(u, vec3(0, 1, 0))
		: cross(u, v);
}

// -------------------------------------------------------
//
//   Calculate soft shadows by tracing a ray (cone)
//    through the mipmap levels of the voxel map
//
// -------------------------------------------------------

// Returns a soft shadow blend by using shadow cone tracing.
// Uses 2 samples per step, so it's pretty expensive.
float TraceConeShadow(
	vec3 from,
	vec3 direction,
	float targetDistance)
{
	from += Normal * 0.05f; // Removes artifacts but makes self shadowing for dense meshes meh.

	float acc = 0;
	float dist = 3 * voxelSize;

	// I'm using a pretty big margin here since I use an emissive light ball with a pretty big radius in my demo scenes.
	float STOP = targetDistance - 16 * voxelSize;

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
		dist += 0.9 * voxelSize * (1 + 0.05 * l);
	}

	return 1 - pow(smoothstep(0, 1, acc * 1.4), 1.0 / 1.4);
}

vec4 TraceCone(
	vec3 origin,       // Origin of cone
	vec3 direction,    // Direction of cone (normalized)
	float ratio,       // Ratio of cone diameter to height (2 = 90deg)
	float maxDistance) // Max distance to cast cone
{
	vec4 color = vec4(0.0f);

 	float distance = voxelSize; // push out the starting point to avoid self-intersection

	while (distance <= maxDistance
		&& color.w  <  1.0f)
	{
		float diameter = max(voxelSize, ratio * distance);
		float LOD      = log2(diameter * voxelSizeInv);

		vec3  position = scaleAndBias(origin + direction * distance);
		vec4  sample_  = textureLod(mat_voxelMap, position, LOD);
		float weight   = 1.0f - color.w;

		color    += sample_ * weight;
		distance += diameter;
	}

	// decompress color range to decode limited HDR. Is this scuffed gamma correction?
	//color.xyz *= 2.0;

	return color;
}

vec3 indirectDiffuseLight(
	vec3 N,            // Normal     of fragment
	vec3 T,            // Tangent    of fragment
	vec3 B,            // Bitangent  of fragment
	vec3 W,            // WorldPos   of fragment
	vec3 baseColor)    // base color of fragment (rgb)
{
	vec3 diffuse = vec3(0.0f);

	float coneRatio   = 1.0f;
	float maxDistance = 2.0f;

	vec3 tan  = normalize(N + T);
	vec3 btan = normalize(N + B);

	diffuse += TraceCone(W, N, coneRatio, maxDistance).xyz;

	diffuse += ISQRT2 * TraceCone(W,  tan,  coneRatio, maxDistance).xyz;
	diffuse += ISQRT2 * TraceCone(W, -tan,  coneRatio, maxDistance).xyz;
	diffuse += ISQRT2 * TraceCone(W,  btan, coneRatio, maxDistance).xyz;
	diffuse += ISQRT2 * TraceCone(W, -btan, coneRatio, maxDistance).xyz;

	return diffuse;
}

// -------------------------------------------------------
//
//   Calculate direct lighting with a simple phong brdf
//
// -------------------------------------------------------

vec3 BRDF_phong(
	vec3 N,            // Normal of fragment
	vec3 V,            // Direction from camera to fragment
	vec3 L,            // Direction from light  to fragment
	vec3 baseColor,    // base color  of fragment (rgb)
	float reflectance, // reflectance of fragment 
	int blinn)         // blinn-phong or normal phong (1 or !1)
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
		diff = min(diff, TraceConeShadow(WorldPos, nL, length(L)));
	}

	//vec3 specular = vec3(1.0) * spec; // bright white light color. should pass light color
	return baseColor * diff;
}

void main() {
	// Material properties
	vec4  baseColor   = mat_baseColor;
	float reflectance = mat_reflectance;

	vec4 color = vec4(0, 0, 0, 1);

	vec3 N = normalize(Normal);
	vec3 T = normalize(Tangent);
	vec3 B = normalize(BiTangent);
	vec3 V = normalize(CameraPos - WorldPos);

	for (int i = 0; i < pointLightCount; i++) {
		float R = pointLights[i].Radius;
		vec3 P  = pointLights[i].Position;
		vec3 L  = P - WorldPos;

		color.xyz += BRDF_phong(N, V, L, baseColor.xyz, reflectance, 0) * DistanceAttenuation(L, R);
		color.xyz += indirectDiffuseLight(N, T, B, WorldPos, baseColor.xyz);
	}

	PixelColor = color;
}
