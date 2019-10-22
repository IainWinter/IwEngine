#pragma once

#include "IwRenderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace IwRenderer {
	class IWRENDERER_API IVertexArray {
	public:
		virtual ~IVertexArray() {}

		virtual void AddBuffer(
			IVertexBuffer* vbo,
			const VertexBufferLayout& layout) = 0;  // Gives state to infterface?
	protected:
		IVertexArray() {}
	};
}
