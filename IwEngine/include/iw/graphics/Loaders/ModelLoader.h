#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Model.h"
#include "iw/asset/AssetLoader.h"

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API MeshLoader
		: public AssetLoader<Model>
	{
	public:
		MeshLoader(
			AssetManager& asset);
	private:
		Model* LoadAsset(
			std::string filepath) override;
	};
}
}

