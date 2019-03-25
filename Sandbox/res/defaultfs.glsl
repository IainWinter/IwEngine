#version 430 core

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform float specularScale;

in vData {
	vec3 Position;
	vec3 Normal;
} frag;

void main() {
	float ambientStrength = 0.1;
	vec3 objectColor = vec3(1, 1, 1);
	
	vec3 viewPos = vec3(0, 0, 0);

	vec3 ambient = ambientStrength * lightColor;

	vec3 norm = normalize(frag.Normal);
	vec3 lightDir = normalize(lightPos - frag.Position);

	vec3 viewDir = normalize(viewPos - frag.Position);
	vec3 reflectDir = reflect(-lightDir, norm);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * lightColor;

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularScale * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	gl_FragColor = vec4(result, 1.0);
}
