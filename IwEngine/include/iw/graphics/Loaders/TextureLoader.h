#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Texture.h"
#include "iw/asset/AssetLoader.h"

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API TextureLoader
		: public AssetLoader<Texture>
	{
	public:
		TextureLoader(
			AssetManager& asset);

	private:
		Texture* LoadAsset(
			std::string filepath) override;

		void FreeAsset(
			Texture* texture) override;
	};
}
}

