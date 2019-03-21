#shader vertex
#version 430 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

layout(location = 0) out vec3 Normal;
layout(location = 1) out vec3 FragPos;

layout(location = 0) uniform mat4 u_world;
layout(location = 4) uniform mat4 u_view;
layout(location = 8) uniform mat4 u_projection;

void main() {
	gl_Position = u_view * u_projection * u_world * vec4(aPosition, 1);
	FragPos = vec3(u_view * vec4(aPosition, 1.0));
	Normal = aNormal;
};

#shader fragment
#version 430 core

layout(location = 0) in vec3 Normal;
layout(location = 1) in vec3 FragPos;

layout(location = 0) out vec4 FragColor;

void main() {
	vec3 lightPos = vec3(1, 1, 1);
	vec3 lightColor = vec3(1, 1, 1);
	vec3 objectColor = vec3(0.5f, 0.5f, 0.5f);

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	vec3 result = (diffuse) * objectColor;
	FragColor = vec4(result, 1.0);
};
