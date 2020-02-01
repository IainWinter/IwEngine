#shader Vertex
#version 440 core

layout(std140, column_major) uniform Camera {
	mat4 viewProj;
};

in vec3 vert;
in vec2 uv;

out vec2 TexCoords;

void main() {
	TexCoords = uv;
	gl_Position = vec4(vert, 1);
}

#shader Fragment
#version 440 core

in vec2 TexCoords;

uniform sampler2D filterTexture;

void main() {
	gl_FragColor = texture2D(filterTexture, TexCoords);
}
