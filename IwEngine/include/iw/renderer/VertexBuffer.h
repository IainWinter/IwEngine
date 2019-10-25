#pragma once

#include "IwRenderer.h"

namespace IW {
inline namespace Renderer {
	class IWRENDERER_API IVertexBuffer {
	public:
		virtual ~IVertexBuffer() {}
	protected:
		IVertexBuffer() {}
	};
}
}
