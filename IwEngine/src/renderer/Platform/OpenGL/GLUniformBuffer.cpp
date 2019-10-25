#include "iw/renderer/Platform/OpenGL/GLUniformBuffer.h"
#include "gl/glew.h"

namespace IW {
	GLUniformBuffer::GLUniformBuffer(
		unsigned int size, 
		const void* data)
	{
		glGenBuffers(1, &m_renderId);
		glBindBuffer(GL_ARRAY_BUFFER, m_renderId);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW); // TODO: Add way to pass this as arg
	}

	GLUniformBuffer::~GLUniformBuffer() {
		glDeleteBuffers(1, &m_renderId);
	}

	void GLUniformBuffer::UpdateData(
		unsigned int size,
		const void* data) const
	{
		glNamedBufferSubData(m_renderId, 0, size, data);
	}

	void GLUniformBuffer::Bind() const {
		glBindBuffer(GL_UNIFORM_BUFFER, m_renderId);
	}

	void GLUniformBuffer::Unbind() const {
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}
