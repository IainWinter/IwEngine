#include "iw/renderer/Platform/OpenGL/GLVertexArray.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	GLVertexArray::GLVertexArray() {
		glGenVertexArrays(1, &gl_id);
		glBindVertexArray(gl_id); // prob not
	}

	GLVertexArray::~GLVertexArray() {
		glDeleteVertexArrays(1, &gl_id);
		
		for (GLVertexBuffer* buffer : m_buffers) {
			delete buffer;
		}

		//m_buffers.clear();
		//m_layouts.clear();
	}

	void GLVertexArray::AddBuffer(
		GLVertexBuffer* vb, 
		const VertexBufferLayout& layout) 
	{
		Bind();
		vb->Bind();
		
		unsigned offset = 0;
		unsigned index  = (unsigned)m_buffers.size();
		for (const VertexBufferLayoutElement& element : layout.GetElements()) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(
				index,
				element.Count, 
				element.Type,
				element.Normalized, 
				layout.GetStride(), 
				(const void*)offset);

			offset += element.Count * GetSizeOfType(element.Type);
			++index;
		}

		Unbind();
		vb->Unbind();

		m_buffers.push_back(vb);
		m_layouts.push_back(layout);
	}

	void GLVertexArray::UpdateBuffer(
		size_t index, 
		const void* data,
		size_t size,
		size_t offset)
	{
		m_buffers[index]->UpdateData(data, size, offset);
	}

	void GLVertexArray::Bind() const {
		glBindVertexArray(gl_id);
	}

	void GLVertexArray::Unbind() const {
		glBindVertexArray(0);
	}
}
}
