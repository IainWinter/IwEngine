#include "iw/graphics/IndexBuffer.h"
#include "gl/glew.h"

namespace IwGraphics {
	IndexBuffer::IndexBuffer(
		const void* data, 
		unsigned int count) 
		: m_count(count)
		, m_data(data) 
	{
		glGenBuffers(1, &m_renderId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, count 
			* sizeof(unsigned int), data, GL_STATIC_DRAW);
	}

	IndexBuffer::~IndexBuffer() {
		glDeleteBuffers(1, &m_renderId);
		delete m_data;
	}

	void IndexBuffer::Bind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderId);
	}

	void IndexBuffer::Unbind() const {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void IwGraphics::IndexBuffer::Draw() const {
		glDrawElements(GL_TRIANGLES, m_count, GL_UNSIGNED_INT, nullptr);
	}
}
