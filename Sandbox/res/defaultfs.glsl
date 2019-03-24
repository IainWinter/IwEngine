#version 430 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

void main() {
	float ambientStrength = 0.1;
	float specularStrength = 0.5f;
	vec3 objectColor = vec3(1, 1, 1);
	vec3 lightColor = vec3(1, 1, 1);
	vec3 lightPos = vec3(0, 0, -5);
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
