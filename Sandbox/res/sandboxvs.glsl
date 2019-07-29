#version 430 core

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

in vec3 position;

void main() {
	gl_Position =  proj * view * model * vec4(position, 1);
}
