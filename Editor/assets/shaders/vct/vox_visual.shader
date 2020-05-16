#shader Vertex
#version 430 core

layout(std140, column_major) uniform Camera {
	mat4 viewProj;
	vec4 camPos;
};

layout (location = 0) in vec3 vert;
out vec2 textureCoordinateFrag;
out vec3 CameraPos;

vec2 scaleAndBias(vec2 p) { return 0.5f * p + vec2(0.5f); }

void main() {
	textureCoordinateFrag = scaleAndBias(vert.xy);
	gl_Position = vec4(vert, 1);
}

#shader Fragment
#version 430

#define INV_STEP_LENGTH (1.0f/STEP_LENGTH)
#define STEP_LENGTH 0.005f

uniform sampler3D texture3D; // Texture in which voxelization is stored.
uniform int state = 0; // Decides mipmap sample level.

in vec3 CameraPos; // World camera position.
in vec2 textureCoordinateFrag;

out vec4 color;

// Scales and bias a given vector (i.e. from [-1, 1] to [0, 1]).
vec3 scaleAndBias(vec3 p) { return 0.5f * p + vec3(0.5f); }

// Returns true if p is inside the unity cube (+ e) centered on (0, 0, 0).
bool isInsideCube(vec3 p, float e) { return abs(p.x) < 1 + e && abs(p.y) < 1 + e && abs(p.z) < 1 + e; }

void main() {
	const float mipmapLevel = state;

	// Initialize ray.
	const vec3 origin = isInsideCube(cameraPosition, 0.2f) ?
		cameraPosition : texture(textureFront, textureCoordinateFrag).xyz;
	vec3 direction = texture(textureBack, textureCoordinateFrag).xyz - origin;
	const uint numberOfSteps = uint(INV_STEP_LENGTH * length(direction));
	direction = normalize(direction);

	// Trace.
	color = vec4(0.0f);
	for (uint step = 0; step < numberOfSteps && color.a < 0.99f; ++step) {
		const vec3 currentPoint = origin + STEP_LENGTH * step * direction;
		vec3 coordinate = scaleAndBias(currentPoint);
		vec4 currentSample = textureLod(texture3D, scaleAndBias(currentPoint), mipmapLevel);
		color += (1.0f - color.a) * currentSample;
	}
	color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
}
