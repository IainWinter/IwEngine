#shader Vertex
#version 420

#include shaders/camera.shader
uniform mat4 model;

layout(location = 0) in vec3 vert;

void main() {
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment
#version 420

void main() {
	float depth = gl_FragCoord.z;

	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float bias = depth * depth + 0.25 * (dx * dx + dy * dy);

	gl_FragColor = vec4(depth, bias, 0, 1);
}
