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

#define MAX_STEPS 32
#define MAX_DIST 100
#define SURF_DIST 0.01

in vec2 FragPos;

out vec4 FragColor;

float GetDist(vec3 p) {
	vec4 s = vec4(0, 1, 6, 1);

	float sphere = length(p - s.xyz) - s.w;
	float plane  = p.y;

	float distance = min(sphere, plane);

	return distance;
}

float RayMarch(vec3 ro, vec3 rd) {
	float d0 = 0.0f;

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

void main() {
	vec2 res = vec2(1.777f, 1);

	vec3 color = vec3(0);

	vec2 uv = (FragPos - 0.5) * res;

	vec3 ro = vec3(0, 1, 0);
	vec3 rd = normalize(vec3(uv.x, uv.y, 1));

	float d = RayMarch(ro, rd);
	d /= 6;

	color = vec3(d);

	FragColor = vec4(color, 1);
}
