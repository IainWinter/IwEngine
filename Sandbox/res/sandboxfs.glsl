#version 430 core

uniform vec3 color;

void main() {
	gl_FragColor = vec4(color, 1.0);
}
