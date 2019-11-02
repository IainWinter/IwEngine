#version 430 core

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec4 diffuse;
uniform vec4 ambient;
uniform vec4 specular;

in vData {
	vec3 Position;
	vec3 Normal;
} frag;

void main() {
	vec3 norm = normalize(frag.Normal);
	vec3 lightDir = normalize(lightPos - frag.Position);

	vec3 viewDir = normalize(viewPos - frag.Position);
	vec3 reflectDir = reflect(-lightDir, norm);

	float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuseC = diff * diffuse;

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec4 specularC = vec4(spec * specular.w * specular.xyz, 1.0);

	vec4 result = .1 * ambient + diffuseC + specularC;
	gl_FragColor = result;
}
