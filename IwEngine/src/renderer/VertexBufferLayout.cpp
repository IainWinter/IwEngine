#include "iw/renderer/VertexBufferLayout.h"
#include "gl/glew.h"

namespace IwRenderer {
	VertexBufferLayout::VertexBufferLayout()
		: m_stride(0)
	{}

	VertexBufferLayout::VertexBufferLayout(
		VertexBufferLayout&& copy) noexcept
		: m_elements(copy.m_elements)
		, m_stride(copy.m_stride)
	{}

	VertexBufferLayout::VertexBufferLayout(
		const VertexBufferLayout& copy)
		: m_elements(copy.m_elements)
		, m_stride(copy.m_stride)
	{}

	VertexBufferLayout& VertexBufferLayout::operator=(
		VertexBufferLayout&& copy) noexcept
	{
		m_elements = copy.m_elements;
		m_stride   = copy.m_stride;
		
		return *this;
	}

	VertexBufferLayout& VertexBufferLayout::operator=(
		const VertexBufferLayout& copy)
	{
		m_elements = copy.m_elements;
		m_stride = copy.m_stride;

		return *this;
	}

	unsigned int VertexBufferLayoutElement::GetSizeOfType(
		unsigned int glType)
	{
		switch (glType) {
			case GL_FLOAT:         return sizeof(GLfloat);
			case GL_UNSIGNED_INT:  return sizeof(GLuint);
			case GL_UNSIGNED_BYTE: return sizeof(GLbyte);
		}

		return 0;
	}

	template<>
	void VertexBufferLayout::Push<float>(
		unsigned int count)
	{
		m_elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_stride += count * VertexBufferLayoutElement
			::GetSizeOfType(GL_FLOAT);
	}

	template<>
	void VertexBufferLayout::Push<unsigned int>(
		unsigned int count)
	{
		m_elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_stride += count * VertexBufferLayoutElement
			::GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void VertexBufferLayout::Push<unsigned char>(
		unsigned int count)
	{
		m_elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_stride += count * VertexBufferLayoutElement
			::GetSizeOfType(GL_UNSIGNED_BYTE);
	}
}

