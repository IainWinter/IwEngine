#include "iw/renderer/Platform/OpenGL/GLBuffer.h"
#include "iw/renderer/Platform/OpenGL/GLTranslator.h"
#include "iw/renderer/Platform/OpenGL/GLErrorCatch.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLBuffer::GLBuffer(
		BufferType type,
		BufferIOType io,
		const void* data,
		size_t size)
		: gl_id(0)
		, gl_type(TRANSLATE(type))
		, gl_io(TRANSLATE(io))
		, m_type(type)
		, m_io(io)
		, m_size(size) // Set in UpdateData
		, m_data(data)
	{
		GL(glGenBuffers(1, &gl_id));
		Bind();
		GL(glBufferData(gl_type, size, data, gl_io));
	}

	GLBuffer::~GLBuffer() {
		GL(glDeleteBuffers(1, &gl_id));
	}

	void GLBuffer::UpdateData(
		const void* data,
		size_t size,
		size_t offset)
	{
		if (size > m_size) {
			m_size = size;
			GL(glNamedBufferData(gl_id, size, data, gl_io));
		}

		else {
			if (size == 0) {
				size = m_size;
			}

			GL(glNamedBufferSubData(gl_id, offset, size, data));
		}
	}

	void GLBuffer::Bind() const {
		GL(glBindBuffer(gl_type, gl_id));
	}

	void GLBuffer::BindBase(
		unsigned index) const
	{
		GL(glBindBufferBase(gl_type, index, gl_id));
	}

	void GLBuffer::Unbind() const {
		GL(glBindBuffer(gl_type, 0));
	}

	unsigned GLBuffer::Id() const {
		return gl_id;
	}
}
}
