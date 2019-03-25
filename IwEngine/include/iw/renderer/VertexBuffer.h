#pragma once

#include "IwRenderer.h"

namespace IwRenderer {
	class IWRENDERER_API IVertexBuffer {
	public:
		virtual ~IVertexBuffer() {}
	protected:
		IVertexBuffer() {}
	};
}
