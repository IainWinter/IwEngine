#shader Vertex
#version 330

layout(location = 0) in vec3 vert;
layout(location = 1) in vec2 uv;

uniform mat4 model;

out vec2 FragPos;

void main() {
	FragPos = uv;
	gl_Position = model * vec4(vert, 1);
}

#shader Fragment
#version 330

in vec2 FragPos;

uniform sampler2D mat_texture;
uniform float mat_textureScale = 1;
uniform vec2 mat_textureOffset = vec2(0, 0);

out vec4 FragColor;

vec2 gridCount  = vec2(6);
vec2 gridBorder = vec2(.02);
vec2 gridScale = gridCount + gridBorder;

vec4 gridColor = vec4(1, 1, 1, .1);

void main() {
	float dist = 1-length(FragPos*2-1);

	if(dist > .1) {
		vec2 coords = FragPos/mat_textureScale + mat_textureOffset - 0.5/mat_textureScale;
		vec4 color = texture(mat_texture, coords) * dist;

		vec2 gridCoords = FragPos*gridScale;
		vec2 gridOffset = fract(gridCoords);
		vec2 gridLine = 1.0 - step(gridBorder, gridOffset);
		float isGrid = clamp(gridLine.x + gridLine.y, 0.0, 1.0);

		//if(color.a < .1) {
		//	discard;
		//}

		FragColor = mix(color, gridColor, isGrid);
	}

	else if (dist > .08) {
		FragColor = vec4(.5, 0, 1, 1);
	}

	else {
		discard;
	}
}
