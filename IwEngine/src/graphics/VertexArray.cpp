#include "iw/graphics/VertexArray.h"
#include "gl/glew.h"

namespace IwGraphics {
	VertexArray::VertexArray() {
		glGenVertexArrays(1, &m_renderId);
		glBindVertexArray(m_renderId);
	}

	VertexArray::~VertexArray() {
		glDeleteVertexArrays(1, &m_renderId);

		for (std::vector<VertexBuffer*>::iterator it 
			= m_buffers.begin(); it != m_buffers.end(); ++it) 
		{
			delete (*it);
		}

		//for (std::vector<VertexBufferLayout*>::iterator it 
		//	= m_layouts.begin(); it != m_layouts.end(); ++it) 
		//{
		//	delete (*it);
		//}

		m_buffers.clear();
		m_layouts.clear();
	}

	void VertexArray::AddBuffer(
		VertexBuffer* vb, 
		VertexBufferLayout* layout) 
	{
		Bind();
		vb->Bind();
		const auto& elements = layout->GetElements();
		unsigned int offset = 0;
		for (unsigned int i = 0; i < elements.size(); i++) {
			const auto& element = elements[i];
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i, element.Count, element.Type, 
				element.Normalized, layout->GetStride(), (const void*)offset);
			offset += element.Count 
				* VertexBufferLayoutElement::GetSizeOfType(element.Type);
		}

		m_buffers.push_back(vb);
		m_layouts.push_back(layout);
	}

	void VertexArray::Bind() const {
		glBindVertexArray(m_renderId);
	}

	void VertexArray::Unbind() const {
		glBindVertexArray(0);
	}
}
