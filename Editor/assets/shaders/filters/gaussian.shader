#shader Vertex
#version 430 core

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

uniform vec3 blurScale;
uniform sampler2D filterTexture;

void main() {
	vec4 color = vec4(0.0);
	color += texture2D(filterTexture, TexCoords + vec2(-3.0) * blurScale.xy) * 0.00598f;       //1.0  / 64.0;
	color += texture2D(filterTexture, TexCoords + vec2(-2.0) * blurScale.xy) * 0.060626f;	   //6.0  / 64.0;
	color += texture2D(filterTexture, TexCoords + vec2(-1.0) * blurScale.xy) * 0.241843f;	   //15.0 / 64.0;
	color += texture2D(filterTexture, TexCoords + vec2( 0.0) * blurScale.xy) * 0.383103f;	   //20.0 / 64.0;
	color += texture2D(filterTexture, TexCoords + vec2( 1.0) * blurScale.xy) * 0.241843f;	   //15.0 / 64.0;
	color += texture2D(filterTexture, TexCoords + vec2( 2.0) * blurScale.xy) * 0.060626f;	   //6.0  / 64.0;
	color += texture2D(filterTexture, TexCoords + vec2( 3.0) * blurScale.xy) * 0.00598f;	   //1.0  / 64.0;

	gl_FragColor = color;
}
