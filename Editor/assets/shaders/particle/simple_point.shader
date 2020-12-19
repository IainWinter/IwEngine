#shader Vertex
#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;
layout(location = 3) in mat4 i_model;

out vec2 TexCoords;

uniform float time;
uniform mat4 model;

void main() {
	vec4 worldPos = model * i_model * vec4(vert, 1);

	TexCoords = uv;
	gl_Position = worldPos;
}

#shader Fragment
#version 330

in vec2 TexCoords;

out vec4 FragColor;

uniform vec4 mat_color;

uniform float ambiance;

void main() {
	vec4 baseColor = mat_color;

	if (baseColor.a < 0.5f) {
		discard;
	}

	vec3 color = baseColor.rgb + baseColor.rgb * ambiance;

	FragColor = vec4(color, baseColor.a);
}
