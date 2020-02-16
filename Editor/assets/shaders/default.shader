#shader Vertex
#version 440 core

layout(std140, column_major) uniform Camera {
	mat4 viewProj;
};

uniform mat4 model;
uniform mat4 lightSpace;
uniform float mat_hasDisplacementMap;
uniform sampler2D mat_displacementMap;

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;

out vec2 UV;
out vec3 Normal;
out vec4 LightPos;

void main() {
	vec3 disp = vec3(0);
	if (mat_hasDisplacementMap == 1) {
		disp = norm * texture(mat_displacementMap, uv).r;
	}

	vec4 worldpos = model * vec4(vert + disp, 1);

	UV = uv;
	Normal = norm;
	LightPos = lightSpace * worldpos;

	gl_Position = viewProj * worldpos;
}

#shader Fragment
#version 440 core

uniform vec4 mat_albedo;
uniform vec4 mat_ao;
uniform float mat_hasAlbedoMap;
uniform float mat_hasAlphaMaskMap;
uniform float mat_hasAoMap;
uniform float mat_hasShadowMap;

//uniform vec3 ambient;
uniform sampler2D mat_albedoMap;
uniform sampler2D mat_alphaMaskMap;
uniform sampler2D mat_aoMap;
//uniform sampler2D normalMap;
uniform sampler2D mat_shadowMap;

in vec2 UV;
in vec3 Normal;
in vec4 LightPos;

float linstep(float l, float h, float v) {
	return clamp((v - l) / (h - l), 0.0, 1.0);
}

float chebyshevUpperBound(vec2 coords, float distance) {
	vec2 moments = texture(mat_shadowMap, coords).rg;

	if (distance <= moments.x)
		return 1.0;

	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, 0.00002);

	float d = distance - moments.x;
	float p_max = linstep(0.2, 1.0, variance / (variance + d * d));

	return p_max;
}

void main() {
	vec3 normal = Normal;
	/*normal = texture(normalMap, UV).xyz;*/

	vec4 diffuse = mat_albedo;
	if (mat_hasAlbedoMap == 1) {
		diffuse *= texture(mat_albedoMap, UV);
	}

	if (mat_hasAlphaMaskMap == 1) {
		if (0.5 > texture(mat_alphaMaskMap, UV).r) {
			discard;
		}
		//diffuse.a = texture(mat_alphaMaskMap, UV).r;
	}

	vec4 ao = mat_ao;
	if (mat_hasAoMap == 1) {
		ao.a *= texture(mat_aoMap, UV).r;
	}

	float ambiance = 0.01;

	float shadow = 0;
	if (mat_hasShadowMap == 1) {
		vec3 coords = (LightPos.xyz / LightPos.w) * 0.5 + 0.5;
		shadow = chebyshevUpperBound(coords.xy, coords.z);
	}

	vec4 color = ambiance /* ambient*/ * ao + diffuse * vec4(vec3(ambiance + shadow), 1);

	gl_FragColor = color;

	if (color.x == 0.05345) {
		gl_FragColor = vec4(normal, 1);
	}
}
