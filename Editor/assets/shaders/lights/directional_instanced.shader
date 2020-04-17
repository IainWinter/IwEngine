#shader Vertex
#version 330

layout(std140, column_major) uniform Camera {
	mat4 viewProj;
	vec4 camPos;
};

layout (location = 0) in vec3 vert;
layout (location = 1) in mat4 model;

void main() {
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment
#version 330

void main() {
	float depth = gl_FragCoord.z;

	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float bias = depth * depth + 0.25 * (dx * dx + dy * dy);

	gl_FragColor = vec4(depth, bias, 0, 1);
}
