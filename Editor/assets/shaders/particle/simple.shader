#shader Vertex
#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in mat4 i_model;
layout(location = 4) in mat4 i_modelViewProj;

out vec2 TexCoords;

#include shaders/shadows.vert

void main() {
	vec4 worldPos = i_model * vec4(vert, 1);
	SetDirectionalLightPos(worldPos);

	TexCoords = uv;
	gl_Position = i_modelViewProj * vec4(vert, 1);
}

#shader Fragment
#version 330

in vec2 TexCoords;

out vec4 FragColor;

uniform vec4 mat_baseColor;

uniform float     mat_hasDiffuseMap;
uniform sampler2D mat_diffuseMap;

uniform float     mat_hasShadowMap;       // take out of material at some point
uniform sampler2D mat_shadowMap;          // take out of material at some point

uniform float ambiance;

#include shaders/shadows.frag
#include shaders/lights.shader

void main() {
	vec4 baseColor = mat_baseColor;

	if (mat_hasDiffuseMap == 1.0f) {
		baseColor = texture(mat_diffuseMap, TexCoords);
	}

	if (baseColor.a < 0.5f) {
		discard;
	}

	vec3 color = baseColor.rgb * ambiance;

	for (int i = 0; i < lightCounts.y; i++) {
		color += baseColor.rgb * DirectionalLightShadow(DirectionalLightPos[i]);
	}

	FragColor = vec4(color, baseColor.a);
}
