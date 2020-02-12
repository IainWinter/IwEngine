#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Shader.h"
#include "iw/asset/AssetLoader.h"

namespace iw {
namespace Graphics {
	class ShaderLoader
		: public AssetLoader<Shader>
	{
	public:
		IWGRAPHICS_API ShaderLoader(
			AssetManager& asset);
	private:
		IWGRAPHICS_API Shader* LoadAsset(
			std::string filepath) override;
	};
}

	using namespace Graphics;
}
