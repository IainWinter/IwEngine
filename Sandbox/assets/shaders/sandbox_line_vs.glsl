#version 430 core

layout (std140, row_major) uniform Camera {
	mat4 proj;
	mat4 view;
};

uniform mat4 model;

in vec3 position;
in vec4 color;

out vec4 fragColor;

void main() {
	gl_Position = proj * view * model * vec4(position, 1);
	fragColor = color;
}
