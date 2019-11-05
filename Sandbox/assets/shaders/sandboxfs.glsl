#version 440 core

in vec2 TexCoords;

uniform sampler2D albedoMap;

void main() {
	float color = texture(albedoMap, TexCoords).r;

	gl_FragColor = vec4(color, color, color, 1.0);
}
