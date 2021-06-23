#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Mesh.h"
#include "iw/asset/AssetLoader.h"

namespace iw {
namespace Graphics {
	class MeshLoader
		: public AssetLoader<Mesh>
	{
	public:
		IWGRAPHICS_API
		MeshLoader(AssetManager& asset) : AssetLoader(asset) {}
	private:
		IWGRAPHICS_API
		Mesh* LoadAsset(std::string filepath) override;
	};
}

	using namespace Graphics;
}

