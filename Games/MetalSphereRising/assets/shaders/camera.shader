layout(std140, column_major) uniform Camera {
	mat4 view;
	mat4 proj;
	mat4 viewProj;
	vec4 camPos;
};
