#pragma once

#include "iw/renderer/UniformBuffer.h"
#include "GLBuffer.h"

namespace IW {
namespace RenderAPI {
	class GLUniformBuffer
		: public IUniformBuffer
		, public GLBuffer
	{
	public:
		IWRENDERER_API
		GLUniformBuffer(
			const void* data,
			size_t size,
			BufferIOType io = DYNAMIC);

		IWRENDERER_API
		~GLUniformBuffer() = default;
	};
}

	using namespace RenderAPI;
}
