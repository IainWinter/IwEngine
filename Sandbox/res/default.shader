#shader vertex
#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

layout(location = 0) out vec3 outColor;

layout(location = 0) uniform mat4 u_world;
layout(location = 4) uniform mat4 u_view;
layout(location = 8) uniform mat4 u_projection;

void main() {
	gl_Position = u_view * u_projection * u_world * vec4(position, 1);
	outColor = color;
};

#shader fragment
#version 430 core

layout(location = 0) out vec4 fragColor;

layout(location = 0) in vec3 ourColor;

void main() {
	fragColor = vec4(ourColor, 1);
};
