#shader Vertex
#version 330

layout(location = 1) in mat4 i_modelViewProj;
layout(location = 2) in vec3 vert;

void main() {
	gl_Position = i_modelViewProj * vec4(vert, 1);
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
