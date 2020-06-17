#shader Vertex
#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in mat4 i_model;

out vec2 TexCoords;

uniform float time;

#include shaders/camera.shader
#include shaders/shadows.vert

mat4 rotationX(float angle) {
	return mat4(1.0,		0,			0,			0,
			 	0, 	cos(angle),	-sin(angle),		0,
				0, 	sin(angle),	 cos(angle),		0,
				0, 			0,			  0, 		1);
}

mat4 rotationY(float angle) {
	return mat4(cos(angle),		0,		sin(angle),	0,
			 			0,		1.0,			 0,	0,
				-sin(angle),	0,		cos(angle),	0,
						0, 		0,				0,	1);
}

mat4 rotationZ(float angle) {
	return mat4(cos(angle),		-sin(angle),	0,	0,
			 	sin(angle),		cos(angle),		0,	0,
						0,				0,		1,	0,
						0,				0,		0,	1);
}

void main() {
	vec4 worldPos = i_model/* * rotationZ(sin(time * 0.01f * uv.y * 1 / sin(time * 0.01f * uv.x)))*/ * vec4(vert, 1);
	SetDirectionalLightPos(worldPos);

	TexCoords = uv;
	gl_Position = viewProj * worldPos;
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

float DirectionalLightShadow2(
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
		color += baseColor.rgb * DirectionalLightShadow2(DirectionalLightPos[i]);
	}

	FragColor = vec4(color, baseColor.a);
}
