#shader Vertex
#version 430 core

layout(std140, column_major) uniform Camera{
	mat4 proj;
	mat4 view;
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

uniform vec3 blurScale;
uniform sampler2D filterTexture;

void main() {
	vec4 color = vec4(0.0);

	color += texture2D(filterTexture, TexCoords + vec2(-3.0) * blurScale.xy) * 1.0 / 64.0;
	color += texture2D(filterTexture, TexCoords + vec2(-2.0) * blurScale.xy) * 6.0 / 64.0;
	color += texture2D(filterTexture, TexCoords + vec2(-1.0) * blurScale.xy) * 15.0 / 64.0;
	color += texture2D(filterTexture, TexCoords + vec2(0.0) * blurScale.xy) * 20.0 / 64.0;
	color += texture2D(filterTexture, TexCoords + vec2(1.0) * blurScale.xy) * 15.0 / 64.0;
	color += texture2D(filterTexture, TexCoords + vec2(2.0) * blurScale.xy) * 6.0 / 64.0;
	color += texture2D(filterTexture, TexCoords + vec2(3.0) * blurScale.xy) * 1.0 / 64.0;

	gl_FragColor = color;
}
