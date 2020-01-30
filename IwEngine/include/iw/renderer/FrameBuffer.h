#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	class IFrameBuffer {
	public:
		IWRENDERER_API
		virtual ~IFrameBuffer() = default;
	protected:
		IFrameBuffer() {}
	};
}

	using namespace RenderAPI;
}
