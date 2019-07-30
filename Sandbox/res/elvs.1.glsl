#version 430 core

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

in vec3 position;
in vec3 normal;

out vData {
	vec3 Position;
	vec3 Normal;
} vertex;

void main() {
	gl_Position =  proj * view * model * vec4(position, 1);
	vertex.Position = (model * vec4(position, 1.0)).xyz;
	vertex.Normal = mat3(transpose(inverse(model))) * normal;
}
