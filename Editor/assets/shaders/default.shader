#shader Vertex
#version 430 core

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 norm;

out vec3 oNorm;

void main() {
	oNorm = norm;
	gl_Position = proj * view * model * vec4(vert, 1);
}

#shader Fragment
#version 440 core

in vec3 oNorm;

void main() {
	gl_FragColor = vec4(oNorm * 0.5f + 0.5f, 1.0);
}
