#include "iw/renderer/Platform/OpenGL/GLUniformBuffer.h"
#include "gl/glew.h"
#include <cstring>

namespace IW {
	GLUniformBuffer::GLUniformBuffer(
		unsigned int size, 
		const void* data)
		: m_size(size)
		, m_data(data)
	{
		glGenBuffers(1, &m_renderId);
		Bind();

		glBufferData(GL_UNIFORM_BUFFER, size, data, GL_DYNAMIC_DRAW); // TODO: Add way to pass this as arg
	}

	GLUniformBuffer::~GLUniformBuffer() {
		glDeleteBuffers(1, &m_renderId);
	}

	void GLUniformBuffer::UpdateData(
		const void* data) const
	{
		Bind();
		void* buffer = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
		memcpy(buffer, data, m_size);
		glUnmapBuffer(GL_UNIFORM_BUFFER);
	}

	void GLUniformBuffer::BindBase(
		unsigned int index) const
	{
		Bind();
		glBindBufferBase(GL_UNIFORM_BUFFER, index, m_renderId);
	}

	void GLUniformBuffer::Bind() const {
		glBindBuffer(GL_UNIFORM_BUFFER, m_renderId);
	}

	void GLUniformBuffer::Unbind() const {
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
}
