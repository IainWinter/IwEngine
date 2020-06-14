#define MAX_DIRECTIONAL_LIGHTS 4

in vec4 DirectionalLightPos[MAX_DIRECTIONAL_LIGHTS];

// Math functions

float linstep(
	float l,
	float h,
	float v)
{
	return clamp((v - l) / (h - l), 0.0, 1.0);
}

// Shadows

float DirectionalLightShadow(
	vec4 coords4)
{
	if (mat_hasShadowMap == 0) {
		return 1.0f;
	}

	vec3 coords = (coords4.xyz / coords4.w) * 0.5 + 0.5;
	vec2 moments = texture(mat_shadowMap, coords.xy).rg;
	float compare = coords.z;

	if (compare > 1.0) { // test if not having this brantch is faster?
		return 1.0;
	}

	float p = step(compare, moments.x);
	float v = max(moments.y - moments.x * moments.x, 0.00002);

	float d = compare - moments.x;
	float pMax = linstep(0.2, 1.0, v / (v + d * d));

	return min(max(p, pMax), 1.0);
}
