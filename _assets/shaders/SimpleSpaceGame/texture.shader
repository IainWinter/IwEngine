#shader Vertex
#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 uv;

out vec2 FragPos;

void main() {
	FragPos = uv;
	gl_Position = vec4(vert, 1);
}

#shader Fragment
#version 330

in vec2 FragPos;

uniform sampler2D inTexture;

out vec4 FragColor;

void main() {
	vec3 color = texture(inTexture, FragPos).rgb;
	FragColor = vec4(color, 1);
}
