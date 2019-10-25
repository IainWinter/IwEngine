#pragma once

#include "IwRenderer.h"

namespace IW {
inline namespace Renderer {
	class IWRENDERER_API IIndexBuffer {
	public:
		virtual ~IIndexBuffer() {}
	protected:
		IIndexBuffer() {}
	};
}
}
