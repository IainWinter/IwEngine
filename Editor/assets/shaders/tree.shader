#shader Vertex
#version 330

layout(std140, column_major) uniform Camera{
	mat4 viewProj;
	vec4 camPos;
};

uniform mat4 model;
uniform mat4 lightSpace;
uniform float time;

in vec3 vert;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec2 uv;

out vec3 WorldPos;
out vec4 LightPos;
out vec3 CameraPos;
out vec2 TexCoords;
out vec3 Normal;
out mat3 TBN;

//mat4 rotationX(float angle) {
//	return mat4(1.0, 0, 0, 0,
//		0, cos(angle), -sin(angle), 0,
//		0, sin(angle), cos(angle), 0,
//		0, 0, 0, 1);
//}
//
//mat4 rotationY(float angle) {
//	return mat4(cos(angle), 0, sin(angle), 0,
//		0, 1.0, 0, 0,
//		-sin(angle), 0, cos(angle), 0,
//		0, 0, 0, 1);
//}
//
//mat4 rotationZ(float angle) {
//	return mat4(cos(angle), -sin(angle), 0, 0,
//		sin(angle), cos(angle), 0, 0,
//		0, 0, 1, 0,
//		0, 0, 0, 1);
//}

void main() {
	//mat4 rot = rotationY(sin(time / 100));

	vec4 worldPos = model * vec4(vert, 1);

	WorldPos  = worldPos.xyz;
	LightPos  = lightSpace * vec4(WorldPos, 1);
	CameraPos = camPos.xyz;
	TexCoords = uv;

	mat3 modelVector = transpose(inverse(mat3(model)));

	vec3 T = normalize(modelVector * tangent);
	vec3 B = normalize(modelVector * bitangent);
	vec3 N = normalize(modelVector * normal);

	TBN = mat3(T, B, N);
	Normal = normalize(modelVector * normal);

	gl_Position = viewProj * worldPos;
}

#shader Fragment shaders/pbr.fs
