#shader Vertex
#version 420

#include shaders/camera.shader

uniform mat4 model;

layout(location = 0) in vec3 vert;
layout(location = 4) in vec2 uv;

out vec2 UV;

void main() {
	UV = uv;
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment
#version 420

uniform sampler2D mat_diffuseMap;
uniform float mat_hasDiffuseMap;

uniform sampler2D mat_alphaMask;
uniform float mat_hasAlphaMask;
uniform float alphaThreshold;

in vec2 UV;

void main() {
	if ((mat_hasDiffuseMap == 1 && alphaThreshold >= texture(mat_diffuseMap, UV).a) || (mat_hasAlphaMask == 1 && alphaThreshold >= texture(mat_alphaMask, UV).r)) {
		discard;
	}

	float depth = gl_FragCoord.z;

	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float bias = depth * depth + 0.25 * (dx * dx + dy * dy);

	gl_FragColor = vec4(depth, bias, 0, 1);
}
