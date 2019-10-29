#version 430 core

layout (std140, column_major) uniform Camera {
	mat4 proj;
	mat4 view;
};

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
