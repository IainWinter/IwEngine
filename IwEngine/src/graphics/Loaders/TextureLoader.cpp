#include "iw/graphics/Loaders/TextureLoader.h"
//#include "iw/math/vector3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/std_image.h"

namespace iw {
namespace Graphics {
	TextureLoader::TextureLoader(
		AssetManager& asset)
		: AssetLoader(asset)
	{}

	Texture* TextureLoader::LoadAsset(
		std::string filepath)
	{
		int width, height, channels;
		stbi_info(filepath.c_str(), &width, &height, &channels);

		unsigned char* image = nullptr;
		switch (channels) {
			case 1: image = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_grey);       break;
			case 2: image = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_grey_alpha); break;
			case 3: {
				image = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
				channels = 4; // hack to fix trees this is def not the way
				break;
			}
			case 4: image = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);  break;
		}

		if (stbi_failure_reason()) {
			LOG_WARNING << stbi_failure_reason();
		}

		if (image == nullptr) {
			return nullptr;
		}

		int size = width * height * channels;

		unsigned char* colors = new unsigned char[size];
		assert(colors);
		
		memcpy(colors, image, size);

		stbi_image_free(image);

		return new Texture(width, height, TEX_2D, (TextureFormat)channels, UBYTE, REPEAT, LINEAR, NEAREST_LINEAR, colors);
	}

	void TextureLoader::FreeAsset(
		Texture* texture)
	{
		free(texture->Colors());
	}
}
}
