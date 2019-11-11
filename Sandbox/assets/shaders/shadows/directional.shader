#shader Vertex
#version 430 core

layout(std140, column_major) uniform Camera{
	mat4 proj;
	mat4 view;
};

in vec3 vert;

uniform mat4 model;

void main() {
	gl_Position = proj * view * model * vec4(vert, 1);
}

#shader Fragment
#version 430 core

void main() {
	float depth = gl_FragCoord.z;

	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float bias = depth * depth + 0.25 * (dx * dx + dy * dy);

	gl_FragColor = vec4(depth, bias, 0, 1);
}
