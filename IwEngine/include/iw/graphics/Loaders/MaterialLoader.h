#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Material.h"
#include "iw/asset/AssetLoader.h"

namespace IW {
namespace Graphics {
	class MaterialLoader
		: public AssetLoader<Material>
	{
	public:
		IWGRAPHICS_API MaterialLoader(
			AssetManager& asset);
	private:
		IWGRAPHICS_API Material* LoadAsset(
			std::string filepath) override;
	};
}

	using namespace Graphics;
}

