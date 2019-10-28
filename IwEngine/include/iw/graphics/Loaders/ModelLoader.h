#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/graphics/Asset/ModelData.h"
#include "iw/asset/AssetLoader.h"
#include "iw/renderer/Device.h"

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API MeshLoader
		: public AssetLoader<ModelData>
	{
	private:
		iwu::ref<IDevice> m_device;

	public:
		MeshLoader(
			AssetManager& asset,
			iwu::ref<IDevice>& device);
	private:
		ModelData* LoadAsset(
			const char* filepath) override;
	};
}
}

