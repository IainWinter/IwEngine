#pragma once

#include "iw/renderer/UniformBuffer.h"
#include "GLBuffer.h"

namespace iw {
namespace RenderAPI {
	class GLUniformBuffer
		: public IUniformBuffer
		, public GLBuffer
	{
	private:
		static unsigned s_base;

		unsigned m_base;

	public:
		IWRENDERER_API
		GLUniformBuffer(
			const void* data,
			size_t size,
			BufferIOType io = DYNAMIC);

		IWRENDERER_API
		void BindMyBase() const override;

		IWRENDERER_API
		~GLUniformBuffer() = default;
	};
}

	using namespace RenderAPI;
}
