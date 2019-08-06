#include "iw/renderer/Platform/OpenGL/GLVertexArray.h"
#include "gl/glew.h"

namespace IwRenderer {
	GLVertexArray::GLVertexArray() {
		glGenVertexArrays(1, &m_renderId);
		glBindVertexArray(m_renderId);
	}

	GLVertexArray::~GLVertexArray() {
		glDeleteVertexArrays(1, &m_renderId);
		for (auto it = m_buffers.begin(); it != m_buffers.end(); ++it) {
			delete (*it);
		}

		m_buffers.clear();
		m_layouts.clear();
	}

	void GLVertexArray::AddBuffer(
		GLVertexBuffer* vb, 
		const VertexBufferLayout& layout) 
	{
		Bind();
		vb->Bind();
		const auto& elements = layout.GetElements();
		unsigned int offset = 0;
		for (unsigned int i = 0; i < elements.size(); i++) {
			const auto& e = elements[i];
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, e.Count, e.Type, e.Normalized,
				layout.GetStride(), (const void*)offset);
			offset += e.Count * GetSizeOfType(e.Type);
		}

		m_buffers.push_back(vb);
		m_layouts.push_back(layout);
	}

	void GLVertexArray::Bind() const {
		glBindVertexArray(m_renderId);
	}

	void GLVertexArray::Unbind() const {
		glBindVertexArray(0);
	}
}
