#pragma once

#include "IwRenderer.h"

namespace IW {
	inline namespace RenderAPI {
		class IWRENDERER_API IFrameBuffer {
		public:
			virtual ~IFrameBuffer() {}
		protected:
			IFrameBuffer() {}
		};
	}
}
