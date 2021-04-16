#shader Vertex
#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 uv;

uniform mat4 model;

out vec2 FragPos;

void main() {
	FragPos = uv;
	gl_Position = model * vec4(vert, 1);
}

#shader Fragment
#version 330

in vec2 FragPos;

uniform sampler2D mat_texture;
uniform int mat_useAlpha;

out vec4 FragColor;

void main() {
	vec4 color = texture(mat_texture, FragPos);
	if (mat_useAlpha == 1 && color.a < 0.5) {
		discard;
	}

	FragColor = color;
}
