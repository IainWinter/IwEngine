#version 430 core

in vec2 TexCoords;

uniform sampler2D albedoMap;

void main() {
	vec3 color = texture(albedoMap, TexCoords).rgb;

	gl_FragColor = vec4(TexCoords, 1.0);
}
