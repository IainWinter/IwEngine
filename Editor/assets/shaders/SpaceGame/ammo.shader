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

out vec4 FragColor;

vec2 gridCount  = vec2(16, 2);
vec2 gridBorder = vec2(.1);
vec2 gridScale = gridCount + gridBorder;

vec4 gridColor = vec4(1, 1, 1, .1);

void main() {
	vec2 gridCoords = FragPos*gridScale;
	vec2 gridOffset = fract(gridCoords);
	vec2 gridLine = 1.0 - step(gridBorder, gridOffset);
	float isGrid = clamp(gridLine.x + gridLine.y, 0.0, 1.0);

	FragColor = mix(vec4(1, 0, 0, 1), gridColor, isGrid);
}
