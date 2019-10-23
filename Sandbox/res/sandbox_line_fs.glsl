#version 430 core

in vec3 fragColor;

void main() {
	gl_FragColor = vec4(fragColor, 1.0);
}
