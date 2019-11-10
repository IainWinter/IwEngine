#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Pipeline.h"
#include "iw/asset/AssetLoader.h"

namespace IW {
	inline namespace Graphics {
		class IWGRAPHICS_API PipelineLoader
			: public AssetLoader<Pipeline>
		{
		private:
			Pipeline* LoadAsset(
				std::string filepath) override;
		};
	}
}

