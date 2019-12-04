#include "iw/renderer/Platform/OpenGL/GLVertexArray.h"
#include "gl/glew.h"

namespace IW {
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
		
		unsigned offset = 0;
		unsigned index  = (unsigned)m_buffers.size();
		for (auto& element : layout.GetElements()) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, element.Count, element.Type,
				element.Normalized, layout.GetStride(), (const void*)offset);

			offset += element.Count * GetSizeOfType(element.Type);
			++index;
		}

		m_buffers.push_back(vb);
		m_layouts.push_back(layout);
	}

	void GLVertexArray::UpdateBuffer(
		size_t index, 
		unsigned size, 
		const void* data)
	{
		m_buffers[index]->UpdateData(size, data);
	}

	void GLVertexArray::Bind() const {
		glBindVertexArray(m_renderId);
	}

	void GLVertexArray::Unbind() const {
		glBindVertexArray(0);
	}
}
