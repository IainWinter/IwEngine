#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Shader.h"
#include "iw/asset/AssetLoader.h"

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API ShaderLoader
		: public AssetLoader<Shader>
	{
	public:
		ShaderLoader(
			AssetManager& asset);
	private:
		Shader* LoadAsset(
			std::string filepath) override;
	};
}
}
