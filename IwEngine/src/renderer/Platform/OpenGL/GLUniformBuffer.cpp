#include "iw/renderer/Platform/OpenGL/GLUniformBuffer.h"
#include "gl/glew.h"
#include <cstring>

namespace IW {
	GLUniformBuffer::GLUniformBuffer(
		size_t size, 
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
		glNamedBufferSubData(m_renderId, 0, m_size, data);


		//Bind();
		//void* buffer = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);  // need way to only copy a lil bit
		//memcpy(buffer, data, m_size);
		//glUnmapBuffer(GL_UNIFORM_BUFFER);
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
