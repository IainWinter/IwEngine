#shader vertex
#version 430 core

layout(location = 0) in vec4 position;

layout(location = 0) uniform mat4 u_world;
layout(location = 4) uniform mat4 u_view;
layout(location = 8) uniform mat4 u_projection;

void main() {
	gl_Position = u_view * u_projection * u_world * position;
};

#shader fragment
#version 430 core

layout(location = 0) out vec4 color;

layout(location = 1) uniform vec4 u_color;

void main() {
	color = vec4(1, 1, 1, 1);
};
