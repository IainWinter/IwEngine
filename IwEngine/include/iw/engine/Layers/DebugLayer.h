#pragma once

#include "iw/engine/Layer.h"

namespace IW {
inline namespace Engine {
	class IWENGINE_API DebugLayer
		: public Layer
	{
	public:
		DebugLayer(
			IW::Space& space,
			IW::Renderer& renderer,
			IW::AssetManager& asset);

		void PostUpdate() override;
		void ImGui() override;
	};
}
}
