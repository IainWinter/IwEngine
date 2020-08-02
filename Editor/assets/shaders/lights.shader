#define MAX_POINT_LIGHTS 16
#define MAX_DIRECTIONAL_LIGHTS 4

struct PointLight {
	vec4 Position; //float Radius;
	vec4 Color;
};

struct DirectionalLight {
	vec4 InvDirection;
	vec4 Color;
};

layout(std140) uniform Lights{
	vec4 lightCounts; // x = point y = direct
	PointLight       pointLights[MAX_POINT_LIGHTS];
	DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
};

float DistanceAttenuation(
	vec3 L,
	float radius)
{
	float radiusInvSqr = 1.0f / pow(radius, 2.0f);
	float distanceSqr = dot(L, L);

	return clamp(1.0f - distanceSqr * radiusInvSqr, 0.0f, 1.0f);
}
