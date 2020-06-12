#shader Vertex
#version 330

layout(location = 0) in vec3 vert;
layout(location = 2) in vec2 uv;
layout(location = 4) in mat4 i_modelViewProj;

out vec2 TexCoords;

void main() {
	TexCoords = uv;
	gl_Position = i_modelViewProj * vec4(vert, 1);
}

#shader Fragment
#version 330

in vec2 TexCoords;

uniform vec4 mat_baseColor;

uniform float     mat_hasDiffuseMap;
uniform sampler2D mat_diffuseMap;

void main() {
	float a = mat_baseColor.a;

	if (mat_hasDiffuseMap == 1.0f) {
		a = texture(mat_diffuseMap, TexCoords).a;
	}

	if (a < 0.5f) {
		discard;
	}

	float depth = gl_FragCoord.z;

	float dx = dFdx(depth);
	float dy = dFdy(depth);
	float bias = depth * depth + 0.25 * (dx * dx + dy * dy);

	gl_FragColor = vec4(depth, bias, 0, 1);
}
