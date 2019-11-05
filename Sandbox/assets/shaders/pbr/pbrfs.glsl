#version 430 core

out vec4 FragColor;

in vec3 WorldPos;
in vec4 LightPos;
in vec2 TexCoords;
in vec3 Normal;
in mat3 TBN;

// material parameters

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

//uniform vec3  albedo;
//uniform float metallic;
//uniform float roughness;
//uniform float ao;

// lights
uniform vec3 lightPositions[4];
uniform vec3 lightColors[4];
uniform vec3 camPos;

//uniform vec3 sunDirection;
uniform sampler2D shadowMap;

const float PI = 3.14159265359;
 
float CalcShadow() {
	vec3 coords = LightPos.xyz / LightPos.w;
	//vec2 moments = texture(shadowMap, coords.xy).rg;
	float compare = coords.z;

	//float p = step(compare, moments.x);
	//float v = max(moments.y - moments.x * moments.x, 0.00002);

	//float d = compare - moments.x;
	//float pMax = v / (v + d*d);

	//return min(max(p, pMax), 1.0);

	vec2 moments = texture2D(shadowMap, coords.xy).xy;
	
	float p = step(compare, moments.x);
	float variance = moments.y - moments.x * moments.x;
	
	float d = compare - moments.x;
	float pMax = variance / (variance + d*d);
	
	return min(max(p, pMax), 1.0);

	//vec3 projCoords = (LightPos.xyz / LightPos.w) * .5 + .5;
	//float closestDepth = texture(shadowMap, projCoords.xy).r;
	//float currentDepth = projCoords.z;

	//float shadow = currentDepth - 0.005 > closestDepth ? 1.0 : 0.0;

	//return 1 - shadow;
}

float DistributionGGX(vec3 N, vec3 H, float roughness) {
	float a	  = roughness*roughness;
	float a2	 = a*a;
	float NdotH  = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;
	
	float num   = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;
	
	return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float num   = NdotV;
	float denom = NdotV * (1.0 - k) + k;
	
	return num / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2  = GeometrySchlickGGX(NdotV, roughness);
	float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	vec3 albedo = texture(albedoMap, TexCoords).rgb;
	vec3 normal = texture(normalMap, TexCoords).rgb * 2.0 - 1.0;
	float metallic  = texture(metallicMap, TexCoords).r;
	float roughness = texture(roughnessMap, TexCoords).r;
	float ao = texture(aoMap, TexCoords).r;

	normal = normalize(TBN * normal);

	vec3 N = normalize(normal);
	vec3 V = normalize(camPos - WorldPos);

	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metallic);

	float realRoughness = roughness * roughness;
			   
	// reflectance equation
	vec3 Lo = vec3(0.0);
	for(int i = 0; i < 4; ++i) 
	{
		// calculate per-light radiance
		vec3 L = normalize(lightPositions[i] - WorldPos);
		vec3 H = normalize(V + L);
		float distance	= length(lightPositions[i] - WorldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance	 = lightColors[i] * attenuation;		
		
		// cook-torrance brdf
		float NDF = DistributionGGX(N, H, realRoughness);		
		float G   = GeometrySmith(N, V, L, realRoughness);	  
		vec3 F	= fresnelSchlick(max(dot(H, V), 0.0), F0);	   
		
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;
		kD *= 1.0 - metallic;	  
		
		vec3 numerator	= NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular	 = numerator / max(denominator, 0.001);  
			
		// add to outgoing radiance Lo
		float NdotL = max(dot(N, L), 0.0);				
		Lo += (kD * albedo / PI + specular) * radiance * NdotL; 
	}   
  
	vec3 ambient = vec3(.1) * albedo * ao;
	vec3 color = ambient * CalcShadow() + Lo;

	color = color / (color + vec3(1.0));
	color = pow(color, vec3(1.0/2.2));

	FragColor = vec4(color, 1.0);

	if(color == vec3(0.198734)) {
		FragColor = vec4(color, 1.0);
	}
} 
