#pragma once

#include "iw/renderer/IndexBuffer.h"
#include "GLBuffer.h"

namespace iw {
namespace RenderAPI {
	class GLIndexBuffer
		: public IIndexBuffer
		, public GLBuffer
	{
	private:
		unsigned m_count;

	public:
		IWRENDERER_API
		GLIndexBuffer(
			const void* data,
			unsigned int count,
			BufferIOType io = STATIC);

		IWRENDERER_API
		unsigned Count() const;
	};
}

	using namespace RenderAPI;
}
