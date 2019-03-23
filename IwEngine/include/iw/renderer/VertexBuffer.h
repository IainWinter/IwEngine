#pragma once

#include "IwRenderer.h"

namespace IwRenderer {
	class IWRENDERER_API VertexBuffer {
	public:
		virtual ~VertexBuffer() {}
	protected:
		VertexBuffer() {}
	};
}
