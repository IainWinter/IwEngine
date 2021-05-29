#shader Vertex
#version 330

layout(location = 0) in vec2 vert;

uniform mat4 model;

out vec2 FragPos;

void main() {
	FragPos = uv / vec2(2.0f) + vec2(0.5f);
	gl_Position = model * vec4(vert, 0, 1);
}

#shader Fragment
#version 330

in vec2 FragPos;

uniform sampler2D mat_texture;

out vec4 FragColor;

void main() {
	FragColor = texture(mat_texture, FragPos);
}
