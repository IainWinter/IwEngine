#shader Vertex
#version 450

#include shaders/camera.shader

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 uv;

out vec3 CameraPos;
out vec3 WorldPos;
out vec2 TexCoords;
out mat3 TBN;

uniform mat4 model;

void main() {
	vec4 worldPos = model * vec4(vert, 1);
	mat3 worldNom = transpose(inverse(mat3(model)));

	vec3 T = normalize(worldNom * tangent);
	vec3 B = normalize(worldNom * bitangent);
	vec3 N = normalize(worldNom * normal);

	CameraPos = camPos.xyz;
	WorldPos  = worldPos.xyz;
	TexCoords = uv;
	TBN       = mat3(T, B, N);

	gl_Position = viewProj * worldPos;
}

#shader Fragment
#version 450

#define ISQRT2 0.707106

#include shaders/lights.shader
#include shaders/gamma_correction.shader
#include shaders/vct/bounds.shader

in vec3 CameraPos;
in vec3 WorldPos;
in vec2 TexCoords;
in mat3 TBN;

out vec4 PixelColor;

// Material

uniform vec4 mat_baseColor;
uniform float mat_reflectance;
uniform sampler3D mat_voxelMap;

uniform float mat_hasDiffuseMap;
uniform sampler2D mat_diffuseMap;

uniform float mat_hasNormalMap;
uniform sampler2D mat_normalMap;

uniform float mat_hasReflectanceMap;
uniform sampler2D mat_reflectanceMap;

// Voxel
uniform vec3 voxelBoundsScale;
uniform vec3 voxelBoundsScaleInv;
uniform float voxelSizeInv;
uniform float voxelSize;

// Globals
uniform float ambiance;
uniform int d_state;

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

float TraceConeShadow(
	vec3 origin,       // Origin of cone
	vec3 direction,	   // Direction of cone (normalized)
	float ratio,       // Ratio of cone diameter to height (2 = 90deg)
	float maxDistance)
{
	float shadow = 0;

	float distance = voxelSize * 2;  // Push out origin to avoid self-intersection

	while (distance < maxDistance
		&& shadow   < 1.0f)
	{
		vec3 position = origin + distance * direction;
		
		if (!isInsideCube(position, voxelBoundsScale, 0)) {
			break;
		}

		position = scaleAndBias(position * voxelBoundsScaleInv);

		float diameter = max(voxelSize, ratio * distance);
		float LOD      = log2(diameter * voxelSizeInv);
		float sample1  = 0.062 * textureLod(mat_voxelMap, position, 1 + 0.75 * LOD).a; // idk where these numbers come from xd
		float sample2  = 0.135 * textureLod(mat_voxelMap, position, 4.5      * LOD).a;
		float sample_  = sample1 + sample2;
		float weight   = 1.0f - shadow;

		shadow += sample_ * weight;
		distance += 0.4 * voxelSize * (1 + 0.05 * diameter);
	}

	return 1 - pow(smoothstep(0, 1, shadow * 1.4), 1.0 / 1.4);
}

// -------------------------------------------------------
//
//      Sum color of voxels by tracing a ray (cone)
//      through the mipmap levels of the voxel map
//
// -------------------------------------------------------

vec4 TraceCone(
	vec3 origin,       // Origin of cone
	vec3 direction,    // Direction of cone (normalized)
	float ratio,       // Ratio of cone diameter to height (2 = 90deg)
	float maxDistance) // Max distance to cast cone
{
	vec4 color = vec4(0.0f);

 	float distance = voxelSize * 4; // Push out origin to avoid self-intersection

	while (distance <= maxDistance
		&& color.w  <  1.0f)
	{
		vec3 position = origin + direction * distance;
		
		if (!isInsideCube(position, voxelBoundsScale, 0)) { // Todo: Check if this is worth it
			break;
		}

		position = scaleAndBias(position * voxelBoundsScaleInv);

		float diameter = max(voxelSize, ratio * distance);
		float LOD      = log2(diameter * voxelSizeInv);
		vec4  sample_  = textureLod(mat_voxelMap, position, LOD);
		float weight   = 1.0f - color.w;

		color    += sample_ * weight;
		distance += diameter;
	}

	// decompress color range to decode limited HDR. Is this scuffed gamma correction?
	//color.xyz *= 2.0;

	return color;
}

// -------------------------------------------------------
//
//   Calculate direct lighting with a simple phong brdf
//
// -------------------------------------------------------

vec3 BRDF_phong(
	vec3 N,            // Normal      of fragment
	vec3 V,            // Direction from camera to fragment
	vec3 L,            // Direction from light  to fragment
	vec3 nL,           // Direction from light  to fragment (normalized)
	float reflectance, // Reflectance of fragment
	int blinn)         // blinn-phong or normal phong (1 or !1)
{
	float NdotL = clamp(dot(N, nL), 0.0f, 1.0f);

	float diff = max(NdotL, 0.0);
	float spec = 0.0;

	if (blinn == 1) {
		vec3 halfwayDir = normalize(L + V);
		spec = dot(N, halfwayDir);
	}

	else {
		vec3 reflectDir = reflect(-L, N);
		spec = dot(V, reflectDir);
	}

	spec = max(pow(spec, reflectance * 8.0), 0.0);

	return /*light color*/ vec3(0.3f) * (diff + spec);
}

// -------------------------------------------------------
//
//            Lighting summation functions
//
// -------------------------------------------------------

vec3 directDiffuse(
	vec3 W,            // WorldPos    of fragment
	vec3 N,            // Normal      of fragment
	vec3 V,            // Direction from camera to fragment
	vec3 L,            // Direction from light  to fragment
	float R,           // Radius of light
	float reflectance, // Reflectance of fragment
	int blinn)         // blinn-phong or normal phong (1 or !1)
{
	vec3 nL = normalize(L);

	float coneRatio   = 0.05f;
	float maxDistance = length(L);

	vec3 diffuse = vec3(0.0f);

	diffuse += BRDF_phong(N, V, L, nL, reflectance, 1)
		     * DistanceAttenuation(L, R)
		     * TraceConeShadow(W, nL, coneRatio, maxDistance);
	
	return diffuse;
}

vec3 indirectDiffuse(
	vec3 W,            // WorldPos    of fragment
	vec3 N,            // Normal      of fragment
	vec3 T,            // Tangent     of fragment
	vec3 B)            // Bitangent   of fragment
{
	float coneRatio   = 1.0f;
	float maxDistance = 2.0f;

	vec3 diffuse = vec3(0.0f);

	diffuse +=          TraceCone(W, N,                 coneRatio, maxDistance).xyz;
	diffuse += ISQRT2 * TraceCone(W, normalize(N + T),  coneRatio, maxDistance).xyz;
	diffuse += ISQRT2 * TraceCone(W, normalize(N - T),  coneRatio, maxDistance).xyz;
	diffuse += ISQRT2 * TraceCone(W, normalize(N + B),  coneRatio, maxDistance).xyz;
	diffuse += ISQRT2 * TraceCone(W, normalize(N - B),  coneRatio, maxDistance).xyz;

	return diffuse;
}

vec3 indirectSpecular(
	vec3 W,            // WorldPos   of fragment
	vec3 N,            // Normal     of fragment
	vec3 V)            // Direction from camera to fragment
{
	float coneRatio   = 0.2f;
	float maxDistance = 2.0f;

	// There are atificats at extreme angles, some form of the below if statement will fix it I think

	//if (dot(-V, N) <= 0.1f) { 
	//	return vec3(0.0f);
	//}

	return TraceCone(W, reflect(-V, N), coneRatio, maxDistance).xyz;
}

void main() {
	// Color

	vec4 baseColor = mat_baseColor;
	if (mat_hasDiffuseMap == 1) {
		baseColor = texture(mat_diffuseMap, TexCoords);
	}

	baseColor.rgb = sRGBToLinear(baseColor.rgb);

	// Normal

	vec3 normal = TBN[2];
	if (mat_hasNormalMap == 1) {
		normal = TBN * texture(mat_normalMap, TexCoords).xyz;
	}

	// Reflectance

	float reflectance = mat_reflectance;
	if (mat_hasReflectanceMap == 1) {
		reflectance = texture(mat_reflectanceMap, TexCoords).r;
	}

	vec3 color = vec3(ambiance);

	vec3 T = normalize(TBN[0]);
	vec3 B = normalize(TBN[1]);
	vec3 N = normalize(normal);
	vec3 V = normalize(CameraPos - WorldPos);

	for (int i = 0; i < pointLightCount; i++) {
		float R = pointLights[i].Radius;
		vec3 P  = pointLights[i].Position;
		vec3 L  = P - WorldPos;

		color +=   directDiffuse (WorldPos, N, V, L, R, reflectance, 1);
		color += indirectDiffuse (WorldPos, N, T, B);
		color += indirectSpecular(WorldPos, N, V);
	}

	color *= baseColor.rgb;

	PixelColor = vec4(linearToSRGB(color), baseColor.a);
}
