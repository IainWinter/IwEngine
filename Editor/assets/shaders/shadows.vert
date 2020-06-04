#define MAX_DIRECTIONAL_LIGHTS 4

layout(std140, column_major) uniform Shadows{
	int shadows_pad1, shadows_pad2, shadows_pad3;

	int directionalLightSpaceCount;
	mat4 directionalLightSpaces[MAX_DIRECTIONAL_LIGHTS];
};

out vec4 DirectionalLightPos[MAX_DIRECTIONAL_LIGHTS];

void SetDirectionalLightPos(
	vec4 worldPos)
{
	for (int i = 0; i < directionalLightSpaceCount; i++) {
		DirectionalLightPos[i] = directionalLightSpaces[i] * worldPos;
	}
}
