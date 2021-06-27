#shader Vertex
#version 330

uniform mat4 model;
//uniform mat4 rot_scale;

layout(location = 0) in vec2 vert;
layout(location = 1) in vec2 uv;

out vec2 cellPosition;
out vec2 fragPosition;

void main() {
	vec2 worldpos = (model * vec4(vert, 0, 1)).xy;
	
	cellPosition = worldpos;
	fragPosition = uv;
	
	gl_Position = /*rot_scale **/ vec4(vert, 0, 1); // limited by size of screen
}

#shader Fragment
#version 330

#define MAX_CHUNK_COUNT 100*100

layout(std140, column_major) uniform SandWorld {
	sampler2d chunks[MAX_CHUNK_COUNT];  // [0, 0] is bottom left
	bool isAlive[MAX_CHUNK_COUNT];

	float cellsPerMeter; // 10
	float cellsPerChunk; // 100
	float basePosition;  // -50 * 10 * 100 = -50000
	float chunkSizeInMeters; // 10
};

in vec2 cellPosition;
in vec2 fragPosition;

void main() {	
	vec2 chunkPosition = cellPosition / chunkSizeInMeters;



	// get chunk from cell pos
	// get intra chunk pos from cell pos
	// 
}
