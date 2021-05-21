#shader Compute
#version 430
layout(local_size_x = 16, local_size_y = 8, local_size_z = 1) in;

uniform int level;
layout(binding = 0)        uniform sampler3D source;
layout(binding = 1, rgba8) uniform image3D   target;

void main() {
	ivec3 targetSize = imageSize(target);

	if (   gl_GlobalInvocationID.x >= targetSize.x
		|| gl_GlobalInvocationID.y >= targetSize.y
		|| gl_GlobalInvocationID.z >= targetSize.z)
	{
		return; // out of range
	}

	ivec3 targetPos = ivec3(gl_GlobalInvocationID);
	ivec3 sourcePos = targetPos * 2;

	vec4 color1 = texelFetch(source, sourcePos + ivec3(0, 0, 0), level);
	vec4 color2 = texelFetch(source, sourcePos + ivec3(1, 0, 0), level);
	vec4 color3 = texelFetch(source, sourcePos + ivec3(0, 1, 0), level);
	vec4 color4 = texelFetch(source, sourcePos + ivec3(1, 1, 0), level);
	vec4 color5 = texelFetch(source, sourcePos + ivec3(0, 0, 1), level);
	vec4 color6 = texelFetch(source, sourcePos + ivec3(1, 0, 1), level);
	vec4 color7 = texelFetch(source, sourcePos + ivec3(0, 1, 1), level);
	vec4 color8 = texelFetch(source, sourcePos + ivec3(1, 1, 1), level);

	vec4 color  = color1 + color2 * (1.0 - color1.w); // this was the +X direction?
		 color += color3 + color4 * (1.0 - color3.w);
		 color += color5 + color6 * (1.0 - color5.w);
		 color += color7 + color8 * (1.0 - color7.w);
		 color *= 0.25f;

	imageStore(target, targetPos, color);
}
