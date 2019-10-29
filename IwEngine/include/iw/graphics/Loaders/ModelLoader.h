#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Asset/ModelData.h"
#include "iw/asset/AssetLoader.h"

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API MeshLoader
		: public AssetLoader<ModelData>
	{
	public:
		MeshLoader(
			AssetManager& asset);
	private:
		ModelData* LoadAsset(
			std::string filepath) override;
	};
}
}

