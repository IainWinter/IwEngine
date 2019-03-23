#include "iw/renderer/Platform/OpenGL/GLIndexBuffer.h"
#include "gl/glew.h"

namespace IwRenderer {
	GLIndexBuffer::GLIndexBuffer(
		unsigned int count, 
		const void* data)
		: m_data(data) 
		, m_count(count)
	{
		glGenBuffers(1, &m_renderId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, 
			count * sizeof(unsigned int), data, GL_STATIC_DRAW);
	}

	GLIndexBuffer::~GLIndexBuffer() {
		glDeleteBuffers(1, &m_renderId);
	}

	void GLIndexBuffer::Bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderId);
	}

	void GLIndexBuffer::Unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}
