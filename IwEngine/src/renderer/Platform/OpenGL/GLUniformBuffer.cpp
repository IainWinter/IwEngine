#include "iw/renderer/Platform/OpenGL/GLUniformBuffer.h"
#include "gl/glew.h"
#include <cstring>

namespace iw {
namespace RenderAPI {
	unsigned GLUniformBuffer::s_base = 0;

	GLUniformBuffer::GLUniformBuffer(
		const void* data,
		size_t size,
		BufferIOType io)
		: GLBuffer(UNIFORM, io, data, size)
	{
		m_base = s_base++;
	}

	void GLUniformBuffer::BindMyBase() const {
		GLBuffer::Bind();
		GLBuffer::BindBase(m_base);
	}
}
}
