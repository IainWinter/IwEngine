#include "iw/renderer/Platform/OpenGL/GLVertexBuffer.h"
#include "gl/glew.h"

namespace IW {
	GLVertexBuffer::GLVertexBuffer(
		unsigned int size,
		const void* data)
		: m_size(size)
		, m_data(data)
	{
		glGenBuffers(1, &m_renderId);
		glBindBuffer(GL_ARRAY_BUFFER, m_renderId);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // TODO: Add way to pass this as arg
	}

	GLVertexBuffer::~GLVertexBuffer() {
		glDeleteBuffers(1, &m_renderId);
	}

	void GLVertexBuffer::UpdateData(
		unsigned int size,
		const void* data) const
	{
		glNamedBufferSubData(m_renderId, 0, size, data);
	}

	void GLVertexBuffer::Bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_renderId);
	}

	void GLVertexBuffer::Unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
