#include "iw/graphics/VertexBuffer.h"
#include "gl/glew.h"

namespace IwGraphics {
	VertexBuffer::VertexBuffer(
		const void* data, 
		unsigned int size) 
		: m_data(data) 
	{
		glGenBuffers(1, &m_renderId);
		glBindBuffer(GL_ARRAY_BUFFER, m_renderId);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
	}

	VertexBuffer::~VertexBuffer() {
		glDeleteBuffers(1, &m_renderId);
		delete m_data;
	}

	void VertexBuffer::Bind() const {
		glBindBuffer(GL_ARRAY_BUFFER, m_renderId);
	}

	void VertexBuffer::Unbind() const {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}
