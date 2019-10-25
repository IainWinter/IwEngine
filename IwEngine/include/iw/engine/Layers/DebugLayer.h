#pragma once

#include "iw/engine/Layer.h"

namespace IwEngine {
	class IWENGINE_API DebugLayer
		: public Layer
	{
	public:
		DebugLayer(
			IwEntity::Space& space,
			IW::Graphics::Renderer& renderer);

		void PostUpdate() override;
	};
}
