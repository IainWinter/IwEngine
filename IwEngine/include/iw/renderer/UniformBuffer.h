#pragma once

#include "IwRenderer.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API IUniformBuffer {
	public:
		virtual ~IUniformBuffer() {}
	protected:
		IUniformBuffer() {}
	};
}
}
