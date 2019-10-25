#pragma once

#include "IwRenderer.h"

namespace IW {
inline namespace Renderer {
	class IWRENDERER_API IUniformBuffer {
	public:
		virtual ~IUniformBuffer() {}
	protected:
		IUniformBuffer() {}
	};
}
}
