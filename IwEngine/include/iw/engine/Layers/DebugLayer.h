#pragma once

#include "iw/engine/Layer.h"

namespace IwEngine {
	class IWENGINE_API DebugLayer
		: public Layer
	{
	public:
		DebugLayer(
			IwEntity::Space& space,
			IwGraphics::RenderQueue& renderQueue);

		void PostUpdate() override;
	};
}
