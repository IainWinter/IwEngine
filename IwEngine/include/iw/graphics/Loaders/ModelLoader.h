#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Asset/ModelData.h"
#include "iw/asset/AssetLoader.h"

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API ModelLoader
		: public AssetLoader<ModelData>
	{
	private:
		ModelData* LoadAsset(
			const char* filepath) override;
	};
}
}

