#shader Vertex
#version 430 core

layout(std140, column_major) uniform Camera {
	mat4 viewProj;
};

uniform mat4 model;

in vec3 vert;
in vec2 uv;

out vec2 UV;

void main() {
	UV = uv;
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment
#version 440 core

uniform vec3 color;
uniform sampler2D fontMap;

in vec2 UV;

void main() {
	gl_FragColor = vec4(color, texture2D(fontMap, UV).a);
}
