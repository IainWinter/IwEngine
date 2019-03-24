#version 430 core

in vec3 aPosition;
in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	gl_Position = proj * view * model * vec4(aPosition, 1);
	FragPos = (model * vec4(aPosition, 1.0)).xyz;
	Normal = mat3(transpose(inverse(model))) * aNormal;
}
