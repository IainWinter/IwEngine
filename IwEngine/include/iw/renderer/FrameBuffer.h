#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API IFrameBuffer {
	public:
		virtual ~IFrameBuffer() {}
	protected:
		IFrameBuffer() {}
	};
}

	using namespace RenderAPI;
}
