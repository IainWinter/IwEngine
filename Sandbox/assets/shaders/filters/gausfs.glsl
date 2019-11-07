#version 440 core

in vec2 TexCoords;

uniform sampler2D filterTexture;

void main() {
	gl_FragColor = texture2D(filterTexture, TexCoords);
}
