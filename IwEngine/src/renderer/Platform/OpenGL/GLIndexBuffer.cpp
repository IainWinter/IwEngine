#include "iw/renderer/Platform/OpenGL/GLIndexBuffer.h"
#include "gl/glew.h"

namespace IW {
	GLIndexBuffer::GLIndexBuffer(
		const void* data,
		unsigned count, 
		BufferIOType io)
		: GLBuffer(INDEX, io, data, count * sizeof(unsigned))
		, m_count(count)
	{}

	unsigned GLIndexBuffer::Count() const {
		return m_count;
	}
}

