#shader Vertex
#version 330

layout(location = 0) in vec2 vert;

uniform mat4 model;

out vec2 FragPos;

void main() {
	FragPos = vert / vec2(2.0f) + vec2(0.5f);
	gl_Position = model * vec4(vert, 0, 1);
}

#shader Fragment
#version 330

in vec2 FragPos;

uniform sampler2D mat_texture;
uniform float mat_hasTexture = 0.0f;
uniform vec4 mat_color = vec4(1, 0, 0, 1);

out vec4 FragColor;

void main() {
	if (mat_hasTexture == 0) {
		FragColor = mat_color;		
	}

	else {
		FragColor = texture(mat_texture, FragPos);		
	}
}
