#version 430 core

void main() {
	gl_FragColor = vec4(gl_FragCoord.z, gl_FragCoord.z * gl_FragCoord.z, 0.0, 0.0);
}
