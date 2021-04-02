#shader Vertex
#version 330

uniform mat4 model;

layout(location = 0) in vec3 vert;

void main() {
	gl_Position = model * vec4(vert, 1);
}

#shader Fragment
#version 330

uniform vec4 mat_color;

out vec4 FragColor;

void main() {
	FragColor = mat_color;
}
