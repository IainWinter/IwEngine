#define MAX_POINT_LIGHTS 16
#define MAX_DIRECTIONAL_LIGHTS 4

struct PointLight {
	vec3 Position;
	float Radius;
};

struct DirectionalLight {
	vec3 InvDirection;
};

layout(std140) uniform Lights {
	int lights_pad1, lights_pad2;

	int pointLightCount;
	int directionalLightCount;
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
