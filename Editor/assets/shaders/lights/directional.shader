#shader Vertex
#version 430 core

layout(std140, column_major) uniform Light {
	mat4 viewProjection;
};

uniform mat4 model;

in vec3 vert;

void main() {
	gl_Position = viewProjection * model * vec4(vert, 1);
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
