#version 330

in vec3 WorldPos;
in vec4 LightPos;
in vec3 CameraPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;

out vec4 FragColor;

// global props

uniform vec3 sunPos;

uniform float gamma; // probly not implmented correctly but I tried :c

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
uniform float mat_ao;
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

float DistributionGGX(vec3 N, vec3 H, float roughness) {
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
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

	albedo.xyz = pow(albedo.xyz, vec3(gamma));

	float ao = mat_ao;
	if (mat_hasAoMap == 1) {
		ao = texture(mat_aoMap, TexCoords).r;
	}

	vec3 normal = Normal;
	if (mat_hasNormalMap == 1) {
		normal = TBN * (texture(mat_normalMap, TexCoords).xyz * 2 - 1);
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

	vec3 N = normalize(normal);
	vec3 V = normalize(CameraPos - WorldPos);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo.xyz, metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < 2; ++i) {
		// calculate per-light radiance
		vec3 L = normalize(lightPositions[i] - WorldPos);
		vec3 H = normalize(V + L);
		float distance = length(lightPositions[i] - WorldPos);
		float attenuation = 1.0 / (distance /** distance*/); // not with gamma correction
		vec3 radiance = lightColors[i] * attenuation;

		// cook-torrance brdf
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = numerator / max(denominator, 0.001);

		// add to outgoing radiance Lo
		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo.xyz / PI + specular) * radiance * NdotL;
	}

	// Sun
	{
		// calculate per-light radiance
		vec3 L = normalize(sunPos);
		vec3 H = normalize(V + L);

		// cook-torrance brdf
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;

		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = numerator / max(denominator, 0.001);

		// add to outgoing radiance Lo
		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * albedo.xyz / PI + specular) * CalcShadow();
	}

	vec3 ambient = max(ambiance - ao, 0) * albedo.xyz;
	vec3 color = ambient + Lo;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0 / gamma));

	FragColor = vec4(color, albedo.w);
}
