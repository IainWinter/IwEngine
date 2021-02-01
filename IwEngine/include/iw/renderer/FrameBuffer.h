#pragma once

#include "IwRenderer.h"

namespace iw {
namespace RenderAPI {
	class IFrameBuffer {
	public:
		IWRENDERER_API
		virtual ~IFrameBuffer() = default;

		void ReadPixels(); // trying new api, device should only be for creation / settings
	protected:              // these interfaces are fine to use for function calls
		IFrameBuffer() {}
	};
}

	using namespace RenderAPI;
}
