#shader Vertex
#version 330

#include shaders/shadows.vert

layout(location = 0) in vec3 vert;
layout(location = 1) in mat4 i_model;
layout(location = 2) in mat4 i_modelViewProj;

void main() {
	vec4 worldPos = i_model * vec4(vert, 1);

	SetDirectionalLightPos(worldPos);

	gl_Position = i_modelViewProj * vec4(vert, 1);
}

#shader Fragment
#version 330

out vec4 FragColor;

uniform vec4 mat_color;

uniform sampler2D mat_shadowMap;
uniform float     mat_hasShadowMap;

#include shaders/lights.shader
#include shaders/shadows.frag

void main() {
	vec3 color = vec3(0.0f);

	for (int i = 0; i < directionalLightCount; i++) {
		color += /*mat_color.xyz*/ vec3(1.0f) * DirectionalLightShadow(DirectionalLightPos[i]);
	}

	FragColor = vec4(color, mat_color.a);
}
