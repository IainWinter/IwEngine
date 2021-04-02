#shader Vertex
#version 330

layout (location = 0) in vec3 vert;
layout (location = 1) in vec2 uv;

out vec2 FragPos;

void main() {
	FragPos = uv;
	gl_Position = vec4(vert, 1);
}

#shader Fragment
#version 330

#include shaders/camera.shader

#define MAX_STEPS 100
#define MAX_DIST 100
#define SURF_DIST 0.001

uniform float time;

in vec2 FragPos;

out vec4 FragColor;

// Distance function helpers

float smin(float a, float b, float k)
{
	float h = max(k - abs(a - b), 0.0) / k;
	return min(a, b) - h * h * h * k * (1.0 / 6.0);
}

// Distance functions

float sdSphere(vec3 p, float s)
{
	return length(p) - s;
}

float sdPlane(vec3 p, vec3 n, float h)
{
	return dot(p, n) + h;
}

float sdBox(vec3 p, vec3 b)
{
	vec3 q = abs(p) - b;
	return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float GetDist(vec3 p) {
	vec4 s1 = vec4(0, 1, 6, 1);

	float sphere1 = sdSphere(p - s1.xyz, s1.w);
	
	float box   = sdBox(p - vec3(2, 1, 6), vec3(1, 1, .5));
	float plane = sdPlane(p, vec3(0, 1, 0), 0);

	float distance = min(smin(sphere1, box, (sin(time) + 1)*.5), plane);

	return distance;
}

// Functions that dont need ray marching

vec3 GetNormal(vec3 p) {
	vec2 e = vec2(.01, 0);
	float d = GetDist(p);
	vec3 n = vec3(
		d - GetDist(p - e.xyy),
		d - GetDist(p - e.yxy),
		d - GetDist(p - e.yyx));

	return normalize(n);
}

// Ray marching functions

float RayMarch(vec3 ro, vec3 rd) {
	float d0 = 0.0;

	for (int i = 0; i < MAX_STEPS; i++) {
		vec3 p = ro + rd * d0;
		float dS = GetDist(p);
		d0 += dS;
		if (d0 > MAX_DIST || dS < SURF_DIST) {
			break;
		}
	}

	return d0;
}

float GetLight(vec3 p) {
	vec3 lightPos = vec3(sin(time) * 5, 5, 6 + cos(time) * 5);
	vec3 l = normalize(lightPos-p);
	vec3 n = GetNormal(p);

	float dif = clamp(dot(n, l), 0, 1) + .03;
	float d = RayMarch(p+n*SURF_DIST*1.1, l);
	if (d*d < dot(lightPos, p)) {
		dif *= 0.2;
	}

	return dif;
}

void main() {
	vec2 res = vec2(16/9., 1);
	vec2 uv = (FragPos - 0.5) * res;

	vec3 ro = camPos.xyz;
	vec3 rd = normalize((vec4(uv.x, uv.y, 1, 1) * view).xyz);

	float d = RayMarch(ro, rd);

	vec3 p = ro + rd * d;

	float diff = GetLight(p);
	vec3 color = vec3(diff);
	color = pow(color, vec3(.4545));	// gamma correction
	FragColor = vec4(color, 1);
}
