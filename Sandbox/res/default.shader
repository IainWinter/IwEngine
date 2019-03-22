#shader vertex
#version 430 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;

layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 Normal;

layout(location = 0) uniform mat4 u_model;
layout(location = 1) uniform mat4 u_view;
layout(location = 2) uniform mat4 u_projection;

void main() {
	gl_Position = u_projection * u_view * u_model * vec4(aPosition, 1);
	FragPos = (u_model * vec4(aPosition, 1.0)).xyz;
	Normal = mat3(transpose(inverse(u_model))) * aNormal;
};

#shader fragment
#version 430 core

layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;

layout(location = 0) out vec4 FragColor;

void main() {
	float ambientStrength = 0.1;
	float specularStrength = .5f;
	vec3 objectColor = vec3(.3, .1, .7);
	vec3 lightColor = vec3(.6, .4, .2);
	vec3 lightPos = vec3(5, 0, -5);
	vec3 viewPos = vec3(0, 0, 0);

	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	FragColor = vec4(result, 1.0);
}
