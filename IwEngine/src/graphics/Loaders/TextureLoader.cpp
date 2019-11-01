#include "iw/graphics/Loaders/TextureLoader.h"
#include "iw/math/vector3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/std_image.h"

namespace IW {
	TextureLoader::TextureLoader(
		AssetManager& asset)
		: AssetLoader(asset)
	{}

	Texture* TextureLoader::LoadAsset(
		std::string filepath)
	{
		int width, height, channels;
		unsigned char* image = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb);

		//if (stbi_failure_reason()) {
		//	LOG_WARNING << stbi_failure_reason();
		//	return nullptr;
		//}

		int size = width * height * channels;

		unsigned char* colors = (unsigned char*)malloc(size);
		assert(colors);
		
		memcpy(colors, image, size);

		stbi_image_free(image);

		return new Texture(width, height, channels, colors);
	}

	void TextureLoader::FreeAsset(
		Texture* texture)
	{
		free(texture->Colors);
	}
}
