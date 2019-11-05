#version 430 core

in vec3 vert;

uniform mat4 lightSpace;
uniform mat4 model;

void main() {
	gl_Position = lightSpace * model * vec4(vert, 1);
}
