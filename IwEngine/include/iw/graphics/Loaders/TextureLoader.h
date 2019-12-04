#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Texture.h"
#include "iw/asset/AssetLoader.h"

namespace IW {
inline namespace Graphics {
	class TextureLoader
		: public AssetLoader<Texture>
	{
	public:
		IWGRAPHICS_API TextureLoader(
			AssetManager& asset);

	private:
		IWGRAPHICS_API Texture* LoadAsset(
			std::string filepath) override;

		IWGRAPHICS_API void FreeAsset(
			Texture* texture) override;
	};
}
}

