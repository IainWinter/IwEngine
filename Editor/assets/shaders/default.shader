#shader Vertex
#version 430 core

layout(std140, column_major) uniform Camera {
	mat4 viewProj;
};

uniform mat4 model;
uniform mat4 lightSpace;
//uniform sampler2D displacementMap;

layout(location = 0) in vec3 vert;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 uv;

out vec2 UV;
out vec3 Normal;
out vec4 LightPos;

void main() {
	/*vec3 disp = norm * texture(displacementMap, uv).r;*/
	vec4 worldpos = model * vec4(vert /*+ disp*/, 1);

	UV = uv;
	Normal = norm;
	LightPos = lightSpace * worldpos;

	gl_Position = viewProj * worldpos;
}

#shader Fragment
#version 440 core

layout(std140, column_major) uniform Material {
	vec4 albedo;
	bool hasAlbedoMap;
};

//uniform vec3 ambient;
uniform sampler2D albedoMap;
//uniform sampler2D ambientMap;
//uniform sampler2D normalMap;
uniform sampler2D shadowMap;

in vec2 UV;
in vec3 Normal;
in vec4 LightPos;

float linstep(float l, float h, float v) {
	return clamp((v - l) / (h - l), 0.0, 1.0);
}

float chebyshevUpperBound(vec2 coords, float distance) {
	vec2 moments = texture(shadowMap, coords).rg;

	if (distance <= moments.x)
		return 1.0;

	float variance = moments.y - (moments.x * moments.x);
	variance = max(variance, 0.00002);

	float d = distance - moments.x;
	float p_max = linstep(0.2, 1.0, variance / (variance + d * d));

	return p_max;
}

void main() {
	vec3 normal = Normal;
	/*normal = texture(normalMap, UV).xyz;*/

	vec3 diffuse = albedo.xyz;
	if (hasAlbedoMap) {
		diffuse *= texture(albedoMap, UV).rgb;
	}

	float ao = 0;
	/*ao = texture(ambientMap, UV).r;*/

	float ambiance = 0.05;

	vec3  coords = (LightPos.xyz / LightPos.w) * 0.5 + 0.5;
	float shadow = chebyshevUpperBound(coords.xy, coords.z);

	vec3 color = ambiance /** ambient*/ * ao + diffuse * (ambiance + shadow);

	gl_FragColor = vec4(diffuse, 1);

	if (color == 0.05345) {
		gl_FragColor = vec4(color /** ambient*/ * normal, 1);
	}
}
