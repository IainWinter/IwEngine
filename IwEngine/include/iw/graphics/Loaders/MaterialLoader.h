#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Material.h"
#include "iw/asset/AssetLoader.h"

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API MaterialLoader
		: public AssetLoader<Material>
	{
	public:
		MaterialLoader(
			AssetManager& asset);
	private:
		Material* LoadAsset(
			std::string filepath) override;
	};
}
}

