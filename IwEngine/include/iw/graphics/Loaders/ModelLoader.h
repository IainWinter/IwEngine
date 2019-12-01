#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Model.h"
#include "iw/asset/AssetLoader.h"

namespace IW {
inline namespace Graphics {
	class MeshLoader
		: public AssetLoader<Model>
	{
	public:
		IWGRAPHICS_API MeshLoader(
			AssetManager& asset);
	private:
		IWGRAPHICS_API Model* LoadAsset(
			std::string filepath) override;
	};
}
}

