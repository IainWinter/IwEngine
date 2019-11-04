#version 330 core

// Input vertex data, different for all executions of this shader.
in vec3 vert;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec2 uv;

out vec3 Normal;
out vec3 Tangent;
out vec3 Bitangent;
out vec2 Uv;

// Values that stay constant for the whole mesh.
uniform mat4 depthMVP;

void main() {
	Normal = normal;
	Tangent = tangent;
	Bitangent = bitangent;
	Uv = uv;
	gl_Position =  depthMVP * vec4(vert,1);
}
