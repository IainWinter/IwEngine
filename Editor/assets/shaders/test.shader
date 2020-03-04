#shader Vertex
#version 330

layout(std140, column_major) uniform Camera{
	mat4 viewProj;
	vec4 camPos;
};

uniform mat4 model;

layout (location = 0) in vec3 vert;
layout (location = 1) in vec2 uv;

out vec2 TexCoords;

void main() {
	TexCoords = uv;
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment
#version 330

in vec2 TexCoords;

out vec4 FragColor;

uniform vec4 mat_albedo;
uniform float mat_hasAlbedoMap;
uniform sampler2D mat_albedoMap;

void main() {
	vec4 color = mat_albedo;
	if(mat_hasAlbedoMap == 1.0) {
		color = texture(mat_albedoMap, TexCoords);
	}

	FragColor = color;
}
