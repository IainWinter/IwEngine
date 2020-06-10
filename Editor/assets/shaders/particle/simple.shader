#shader Vertex
#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in mat4 i_model;
layout(location = 4) in mat4 i_modelViewProj;

out vec2 TexCoords;

void main() {
	TexCoords = uv;
	gl_Position = i_modelViewProj * vec4(vert, 1);
}

#shader Fragment
#version 330

in vec2 TexCoords;

out vec4 FragColor;

uniform vec4 mat_baseColor;

uniform float     mat_hasDiffuseMap;
uniform sampler2D mat_diffuseMap;

void main() {
	FragColor.rgb = texture(mat_diffuseMap, TexCoords).rgb;
	FragColor.a = 1;
}
