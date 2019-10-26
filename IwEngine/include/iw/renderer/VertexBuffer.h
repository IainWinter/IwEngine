#pragma once

#include "IwRenderer.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API IVertexBuffer {
	public:
		virtual ~IVertexBuffer() {}
	protected:
		IVertexBuffer() {}
	};
}
}
