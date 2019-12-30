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
	LightPos  = lightSpace * worldpos;
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

float CalcShadow() {
	vec3 coords = (LightPos.xyz / LightPos.w) * 0.5 + 0.5;
	vec2 moments = texture(shadowMap, coords.xy).rg;
	float compare = coords.z;

	if (compare > 1.0) {
		return 1.0;
	}

	float p = step(compare, moments.x);
	float v = max(moments.y - moments.x * moments.x, 0.00002);

	float d = compare - moments.x;
	float pMax = linstep(0.2, 1.0, v / (v + d * d));

	return min(max(p, pMax), 1.0);
}

void main() {
	vec3 c = vec3(.01) + color * CalcShadow();

	c = c / (c + vec3(1.0));
	c = pow(c, vec3(1.0 / 2.2));

	gl_FragColor = vec4(c, 1.0);
}
