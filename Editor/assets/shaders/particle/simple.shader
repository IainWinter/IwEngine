#shader Vertex
#version 330

layout(std140, column_major) uniform Camera {
	mat4 viewProj;
	vec4 camPos;
};

layout (location = 0) in vec3 vert;
layout (location = 1) in mat4 model;

void main() {
	vert.y += gl_InstanceID;
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment
#version 330

out vec4 FragColor;

uniform vec4 mat_color;

void main() {
	FragColor = mat_color;
}
