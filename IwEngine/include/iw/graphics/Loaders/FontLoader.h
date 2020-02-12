#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/asset/AssetLoader.h"
#include "iw/graphics/Font.h"

namespace iw {
namespace Graphics {
	class FontLoader
		: public AssetLoader<Font>
	{
	public:
		IWGRAPHICS_API
		FontLoader(
			AssetManager& asset);
	private:
		IWGRAPHICS_API
		Font* LoadAsset(
			std::string filepath) override;
	};
}

	using namespace Graphics;
}

