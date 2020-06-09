#shader Vertex
#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in mat4 i_model;
layout(location = 4) in mat4 i_modelViewProj;

void main() {
	gl_Position = i_modelViewProj * vec4(vert, 1);
}

#shader Fragment
#version 330

out vec4 FragColor;

uniform vec4 mat_baseColor;

void main() {
	FragColor = mat_baseColor;
}
