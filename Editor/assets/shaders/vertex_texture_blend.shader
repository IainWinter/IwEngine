vec4 blend(sampler2D textureA, sampler2D textureB, vec2 texCoords, float blend) {
	return texture(textureA, texCoords) * vec4(1.0f, 1.0f, 1.0f,        blend)
		+  texture(textureB, texCoords) * vec4(1.0f, 1.0f, 1.0f, 1.0f - blend);
}
