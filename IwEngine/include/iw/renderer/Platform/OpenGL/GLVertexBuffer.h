#pragma once

#include "iw/renderer/VertexBuffer.h"
#include "GLBuffer.h"

namespace iw {
namespace RenderAPI {
	class GLVertexBuffer
		: public IVertexBuffer
		, public GLBuffer
	{
	public:
		IWRENDERER_API
		GLVertexBuffer(
			const void* data,
			size_t size,
			BufferIOType io = STATIC);

		IWRENDERER_API
		~GLVertexBuffer() = default;
	};
}

	using namespace RenderAPI;
}
