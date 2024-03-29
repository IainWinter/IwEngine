#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Model.h"
#include "iw/asset/AssetLoader.h"

namespace iw {
namespace Graphics {
	class ModelLoader
		: public AssetLoader<Model>
	{
	public:
		IWGRAPHICS_API
		ModelLoader(
			AssetManager& asset);
	private:
		IWGRAPHICS_API
		Model* LoadAsset(
			std::string filepath) override;
	};
}

	using namespace Graphics;
}

