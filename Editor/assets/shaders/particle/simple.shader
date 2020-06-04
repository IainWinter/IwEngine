#shader Vertex
#version 330

layout (location = 0) in vec3 vert;
layout (location = 2) in mat4 modelViewProj;

void main() {
	gl_Position = modelViewProj * vec4(vert, 1);
}

#shader Fragment
#version 330

out vec4 FragColor;

uniform vec4 mat_color;

void main() {
	FragColor = mat_color;
}
