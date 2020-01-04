#shader Vertex
#version 430 core

uniform mat4 lightSpace;
uniform mat4 viewProj;
uniform mat4 model;

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;

out vec4 LightPos;
out vec2 UV;

void main() {
	UV = uv;
	vec4 worldpos = model * vec4(vert, 1);
	LightPos = lightSpace * worldpos;
	gl_Position = viewProj * worldpos;
}

#shader Fragment
#version 440 core

uniform vec3 color;
uniform sampler2D shadowMap;

in vec4 LightPos;
in vec2 UV;

float linstep(float l, float h, float v) {
	return clamp((v - l) / (h - l), 0.0, 1.0);
}

float chebyshevUpperBound(vec2 coords, float distance) {
	vec2 moments = texture2D(shadowMap, coords).rg;

	if (distance <= moments.x)
		return 1.0;

	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, 0.00002);

	float d = distance - moments.x;
	float p_max = linstep(0.2, 1.0, variance / (variance + d * d));

	return p_max;
}

float linShadow(vec2 coords, float distance) {
	float lightDepth1 = texture2D(shadowMap, coords).r;
	float lightDepth2 = clamp(distance / 40.0, 0.0, 1.0);
	float bias = 0.001;
	return step(lightDepth2, lightDepth1 + bias);
}

void main() {
	vec3 coords = (LightPos.xyz / LightPos.w) * 0.5 + 0.5;
	float shadow = chebyshevUpperBound(coords.xy, coords.z);

	vec3 c = vec3(.1) + color * shadow;

	//c = c / (c + vec3(1.0));
	//c = pow(c, vec3(1.0 / 2.2));

	gl_FragColor = vec4(c, 1.0);
}
