#shader Vertex
#version 450

#include shaders/camera.shader
#include shaders/shadows.vert

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec3 bitangent;
layout(location = 4) in vec2 uv;
layout(location = 5) in vec4 color;

out vec3 CameraPos;
out vec3 WorldPos;
out vec2 TexCoords;
out mat3 TBN;
out vec4 Color;

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
	Color     = color;

	SetDirectionalLightPos(worldPos);

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
in vec4 Color;

out vec4 PixelColor;

// Material

uniform vec4  mat_baseColor;
uniform float mat_reflectance;
uniform float mat_refractive;

uniform int mat_indirectDiffuse = 1;
uniform int mat_indirectSpecular = 1;

uniform float     mat_hasVoxelMap;
uniform sampler3D mat_voxelMap;

uniform float     mat_hasDiffuseMap;
uniform sampler2D mat_diffuseMap;

uniform float     mat_hasDiffuseMap2; // seperate into another shader?
uniform sampler2D mat_diffuseMap2;

uniform float     mat_hasNormalMap;
uniform sampler2D mat_normalMap;

uniform float     mat_hasNormalMap2;
uniform sampler2D mat_normalMap2;

uniform float     mat_hasReflectanceMap;
uniform sampler2D mat_reflectanceMap;

uniform float     mat_hasShadowMap;       // take out of material at some point
uniform sampler2D mat_shadowMap;          // take out of material at some point

#include shaders/shadows.frag

// Voxel
uniform vec3 voxelBoundsScale;
uniform vec3 voxelBoundsScaleInv;
uniform float voxelSizeInv;
uniform float voxelSize;

// Globals
uniform float ambiance;
uniform vec3 skyColor;
uniform int d_state;

uniform float maxConeLength = 2.0f;

// Settings
uniform int SHADOWS = 0;
uniform int BLINN = 1;

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
		distance += diameter;
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
		&& color.a  <  1.0f)
	{
		vec3 position = origin + direction * distance;
		
		if (!isInsideCube(position, voxelBoundsScale, 0)) {
			break;
		}

		position = scaleAndBias(position * voxelBoundsScaleInv);

		float diameter = max(voxelSize, ratio * distance);
		float LOD      = log2(diameter * voxelSizeInv);
		vec4  sample_  = textureLod(mat_voxelMap, position, LOD);
		float weight   = 1.0f - color.a;

		color    += sample_ * weight;
		distance += diameter;
	}

	color.rgb += skyColor * (1 - color.a);
	color.a = 1.0f;

	return color;
}

// -------------------------------------------------------
//
//                 Lighting functions
//
// -------------------------------------------------------

vec3 directLighting(
	vec3 W,            // WorldPos    of fragment
	vec3 N,            // Normal      of fragment
	vec3 V,            // Direction from camera to fragment
	vec3 L,            // Direction from light  to fragment
	vec3 lightColor,   // Color of light (rgb)
	vec4 baseColor,    // Color of fragment (rgba)
	float reflectance, // Reflectance of fragment
	float refractive)  // Refractive index of fragment
{
	vec3  nL    = normalize(L);
	float NdotL = clamp(dot(N, nL), 0.0f, 1.0f);

	// Diffuse

	float diff = max(NdotL, 0.0f);
	
	// Specular

	float spec = 0.0f;

	if (BLINN == 1) spec = dot(N, normalize(L + V));
	else            spec = dot(V, reflect  (-L, N));

	spec = max(pow(spec, reflectance * 8.0f), 0.0f);

	// Refraction

	float refr = 0.0f;
	
	//if (baseColor.a < 1.0f) {
	//	vec3 refraction = refract(V, N, 1.0f / refractive);
	//	refr = max((1.0f - baseColor.a) * dot(refraction, nL), 0.0f);
	//}

	// Shadow

	float shadow = 1.0f;

	if (   SHADOWS == 1
		&& mat_hasVoxelMap == 1)
	{
		float coneRatio   = 0.05f;
		float maxDistance = 32.0f;

		shadow = TraceConeShadow(W, nL, coneRatio, maxDistance);
	}

	else if (mat_hasShadowMap == 1.0f) {
		shadow = texture(mat_shadowMap, TexCoords).a;
	}
	
	// Mix color

	diff = diff * baseColor.a;
	spec = max(spec, refr);

	return lightColor * (diff + spec) * shadow;
}

vec3 indirectDiffuse(
	vec3 W,            // WorldPos    of fragment
	vec3 N,            // Normal      of fragment
	vec3 T,            // Tangent     of fragment
	vec3 B)            // Bitangent   of fragment
{
	float coneRatio   = 1.0f;
	float maxDistance = maxConeLength;//2.0f;

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
	vec3 V,            // Direction from camera to fragment
	float reflectance) // Reflectance of fragment
{
	float coneRatio   = 0.2f;
	float maxDistance = maxConeLength * 5;//8.0f;

	// There are atificats at extreme angles, some form of the below if statement will fix it I think

	//if (dot(-V, N) <= 0.1f) { 
	//	return vec3(0.0f);
	//}

	if (reflectance == 0.0f) {
		return vec3(0.0f);
	}

	return TraceCone(W, reflect(-V, N), coneRatio, maxDistance).xyz * reflectance;
}

vec3 indirectLighting(
	vec3 W,
	vec3 N,
	vec3 T,
	vec3 B,
	vec3 V,
	float reflectance)
{
	vec3 color = vec3(0);

	if (mat_hasVoxelMap == 1) {
		if (mat_indirectDiffuse  == 1) color += indirectDiffuse (WorldPos, N, T, B);
		if (mat_indirectSpecular == 1) color += indirectSpecular(WorldPos, N, V, reflectance);
	}

	return color;
}

// not used
//vec3 indirectRefractive(
//	vec3 W,            // WorldPos of fragment
//	vec3 N,            // Normal of fragment
//	vec3 V,            // Direction from camera to fragment
//	float refractive)  // Refractive index of fragment  
//{
//	float coneRatio = 0.2f;
//	float maxDistance = 100.0f;
//
//	vec3 refraction = refract(-V, N, 1.0f / refractive);
//	return TraceCone(W, refraction, coneRatio, maxDistance).xyz;
//}

void main() {
	// Color

	vec4 baseColor = mat_baseColor;
	if (mat_hasDiffuseMap == 1) {
		baseColor *= texture(mat_diffuseMap, TexCoords);
		baseColor.rgb = sRGBToLinear(baseColor.rgb);
	}

	if (mat_hasDiffuseMap2 == 1) {
		vec4 baseColor2 = texture(mat_diffuseMap2, TexCoords);
		baseColor2.rgb = sRGBToLinear(baseColor2.rgb);

		baseColor = baseColor  *         Color.r
			      + baseColor2 * (1.0f - Color.r);
	}

	// Normal

	vec3 normal = TBN[2];
	if (mat_hasNormalMap == 1) {
		normal = TBN * texture(mat_normalMap, TexCoords).xyz;
	}

	if (mat_hasNormalMap2 == 1) {
		vec3 normal2 = TBN * texture(mat_normalMap2, TexCoords).xyz;

		normal = normal  *         Color.r
			   + normal2 * (1.0f - Color.r);
	}

	// Reflectance

	float reflectance = mat_reflectance;
	if (mat_hasReflectanceMap == 1) {
		reflectance = texture(mat_reflectanceMap, TexCoords).r;
	}

	// Refractive

	float refractive = mat_refractive; 

	vec3 T = normalize(TBN[0]);
	vec3 B = normalize(TBN[1]);
	vec3 N = normalize(normal);
	vec3 V = normalize(CameraPos - WorldPos);

	vec3 color = vec3(ambiance);

	//for (int i = 0; i < lightCounts.x; i++) {
	//	vec3 P = pointLights[i].Position.xyz;
	//	vec3 L = P - WorldPos;

	//	float lightRadius = pointLights[i].Position.w;
	//	vec3  lightColor  = pointLights[i].Color.rgb;

	//	color += directLighting(WorldPos, N, V, L, lightColor, baseColor, reflectance, refractive)
	//	       * DistanceAttenuation(L, lightRadius);

	//	color += indirectLighting(WorldPos, N, T, B, V, reflectance);
	//}

	for (int i = 0; i < lightCounts.y; i++) {
		vec3 L = directionalLights[i].InvDirection.xyz;

		vec3 lightColor = directionalLights[i].Color.rgb;

		color += directLighting(WorldPos, N, V, L, lightColor, baseColor, reflectance, refractive)
			   * DirectionalLightShadow(DirectionalLightPos[i]);

		color += indirectLighting(WorldPos, N, T, B, V, reflectance);
	}

	//if (baseColor.a < 1.0f) {
	//	color = indirectRefractive(WorldPos, N, V, refractive);
	//}

	color *= baseColor.rgb;

	PixelColor = vec4(linearToSRGB(color), baseColor.a);
}
