#pragma once

#include "IwRenderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace IW {
inline namespace Renderer {
	class IWRENDERER_API IVertexArray {
	public:
		virtual ~IVertexArray() {}

	protected:
		IVertexArray() {}
	};
}
}
