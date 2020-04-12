#shader Vertex
#version 330

layout(std140, column_major) uniform Camera {
	mat4 viewProj;
	vec4 camPos;
};

uniform mat4 model;

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 uv;

out vec2 UV;

void main() {
	UV = uv;
	gl_Position = viewProj * model * vec4(vert, 1);
}

#shader Fragment
#version 330

uniform vec3 mat_color;
uniform float mat_hasFontMap;
uniform sampler2D mat_fontMap;

in vec2 UV;

void main() {
	vec4 color = vec4(mat_color, 1);

	if (mat_hasFontMap == 1) {
		color.a = texture2D(mat_fontMap, UV).a;
	}

	gl_FragColor = color;
}
