// This is annoying to have just for 2 'g's

#define MAX_DIRECTIONAL_LIGHTS 4

layout(std140, column_major) uniform Shadows {
	vec4 directionalLightSpaceCount;
	mat4 directionalLightSpaces[MAX_DIRECTIONAL_LIGHTS];
};

out vec4 gDirectionalLightPos[MAX_DIRECTIONAL_LIGHTS];

void SetDirectionalLightPos(
	vec4 worldPos)
{
	for (int i = 0; i < directionalLightSpaceCount.x; i++) {
		gDirectionalLightPos[i] = directionalLightSpaces[i] * worldPos;
	}
}
