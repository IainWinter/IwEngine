#pragma once

#include "iw/engine/Layer.h"

namespace IwEngine {
	class IWENGINE_API DebugLayer
		: public Layer
	{
	public:
		DebugLayer(
			IwEntity::Space& space,
			IW::Renderer& renderer,
			IW::AssetManager& asset);

		void PostUpdate() override;
		void ImGui() override;
	};
}
