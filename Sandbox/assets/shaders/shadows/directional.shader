#shader Vertex
#version 430 core

in vec3 vert;

uniform mat4 viewProjection;
uniform mat4 model;

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
