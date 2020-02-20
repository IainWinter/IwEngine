#shader Vertex
#version 330

layout(std140, column_major) uniform Camera{
	mat4 viewProj;
	vec4 camPos;
};

uniform mat4 model;
uniform mat4 lightSpace;

// material parameters

uniform float mat_hasDisplacementMap;
uniform sampler2D mat_displacementMap;

in vec3 vert;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec2 uv;

out vec3 WorldPos;
out vec4 LightPos;
out vec3 CameraPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;

void main() {
	vec3 displacement = vec3(0);
	if (mat_hasDisplacementMap == 1) {
		displacement = normal * texture(mat_displacementMap, uv).r;
	}

	vec4 worldPos = model * vec4(vert + displacement, 1);

	WorldPos  = worldPos.xyz;
	LightPos  = lightSpace * vec4(WorldPos, 1);
	CameraPos = camPos.xyz;
	TexCoords = uv;

	mat3 modelVector = transpose(inverse(mat3(model)));

	vec3 T = normalize(modelVector * tangent);
	vec3 B = normalize(modelVector * bitangent);
	vec3 N = normalize(modelVector * normal);
	
	TBN    = mat3(T, B, N);
	Normal = normalize(modelVector * normal);

	gl_Position = viewProj * worldPos;
}

#shader Fragment
#version 330

in vec3 WorldPos;
in vec4 LightPos;
in vec3 CameraPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;

out vec4 FragColor;

// global props

uniform float ambiance;
uniform vec3 lightPositions[2];
uniform vec3 lightColors[2];

// material parameters

uniform float mat_hasAlbedoMap; // guess these have to be floats???? invalid op when using ints idk why
uniform float mat_hasAlphaMaskMap;
uniform float mat_hasNormalMap;
uniform float mat_hasMetallicMap;
uniform float mat_hasRoughnessMap;
uniform float mat_hasAoMap;

uniform sampler2D mat_albedoMap;
uniform sampler2D mat_alphaMaskMap;
uniform sampler2D mat_normalMap;
uniform sampler2D mat_metallicMap;
uniform sampler2D mat_roughnessMap;
uniform sampler2D mat_aoMap;

uniform vec4 mat_albedo;
uniform vec4 mat_ao;
uniform float mat_metallic;
uniform float mat_roughness;

// take out of material at some point, also could be multiple

uniform float mat_hasShadowMap;
uniform sampler2D mat_shadowMap;

const float PI = 3.14159265359f;

float linstep(float l, float h, float v) {
	return clamp((v - l) / (h - l), 0.0, 1.0);
}

float CalcShadow() {
	vec3 coords = (LightPos.xyz / LightPos.w) * 0.5 + 0.5;
	vec2 moments = texture(mat_shadowMap, coords.xy).rg;
	float compare = coords.z;

	if (compare > 1.0) {
		return 1.0;
	}

	float p = step(compare, moments.x);
	float v = max(moments.y - moments.x * moments.x, 0.00002);

	float d = compare - moments.x;
	float pMax = linstep(0.2, 1.0, v / (v + d * d));

	return min(max(p, pMax), 1.0);
}

// google version

float D_GGX(float NoH, float roughness) {
	float a = NoH * roughness;
	float k = roughness / (1.0 - NoH * NoH + a * a);
	return k * k * (1.0 / PI);
}

float V_SmithGGXCorrelated(float NoV, float NoL, float roughness) {
	float a2 = roughness * roughness;
	float GGXV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
	float GGXL = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);
	return 0.5 / (GGXV + GGXL);
}

vec3 F_Schlick(float VoH, vec3 f0) {
	float f = pow(1.0 - VoH, 5.0);
	return f + f0 * (1.0 - f);
}

float Fd_Lambert() {
	return 1.0 / PI;
}

vec3 BRDF(vec3 v, vec3 l, vec3 n, vec3 albedo, vec3 lightColor, float roughness, float metallic) {
	vec3 h = normalize(v + l);

	float distance = length(l);
	float attenuation = 1.0 / (distance * distance);
	vec3 radiance = lightColor * attenuation;

	vec3 f0 = vec3(0.04);
	f0 = mix(f0, albedo, metallic);

	float NoV = abs(dot(n, v)) + 1e-5;
	float NoL = clamp(dot(n, l), 0.0, 1.0);
	float NoH = clamp(dot(n, h), 0.0, 1.0);
	float LoH = clamp(dot(l, h), 0.0, 1.0);

	float D = D_GGX(NoH, roughness);
	float V = V_SmithGGXCorrelated(NoV, NoL, roughness);
	vec3  F = F_Schlick(LoH, f0);

	// specular BRDF
	vec3 Fr = (D * V) * F;

	// diffuse BRDF
	vec3 Fd = albedo * Fd_Lambert();

	return (Fr + Fd) * radiance;
}

void main() {
	if (mat_hasAlphaMaskMap == 1) {
		if (0.5 > texture(mat_alphaMaskMap, TexCoords).r) {
			discard;
		}
	}

	vec4 albedo = mat_albedo;
	if (mat_hasAlbedoMap == 1) {
		albedo = texture(mat_albedoMap, TexCoords);
	}

	vec4 ao = mat_ao;
	if (mat_hasAoMap == 1) {
		ao = texture(mat_aoMap, TexCoords);
	}

	vec3 normal = Normal;
	if (mat_hasNormalMap == 1) {
		normal = texture(mat_normalMap, TexCoords).xyz * 2.0 - 1.0;
	}

	float metallic = mat_metallic;
	if (mat_hasMetallicMap == 1) {
		metallic = texture(mat_metallicMap, TexCoords).r;
	}

	float roughness = mat_roughness;
	if (mat_hasRoughnessMap == 1) {
		roughness = texture(mat_roughnessMap, TexCoords).r;
	}

	float shadow = 1.0f;
	if (mat_hasShadowMap == 1) {
		shadow = CalcShadow();
	}

	float realRoughness = roughness * roughness;

	vec3 N = normalize(TBN * normal);
	vec3 V = normalize(WorldPos - CameraPos);

	vec3 light = vec3(0.0);
	for (int i = 0; i < 2; i++) {
		vec3 L = normalize(WorldPos - lightPositions[i]);

		light += BRDF(V, L, N, albedo.xyz, lightColors[i], realRoughness, metallic);
	}

	vec4 color = ambiance * ao + vec4(light * shadow, 1);

	//color = color / (color + vec4(1.0));
	//color = pow(color, vec3(1.0 / 2.2));

	FragColor = color;
}
