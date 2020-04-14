#include "iw/renderer/VertexBufferLayout.h"
#include "gl/glew.h"

namespace iw {
namespace RenderAPI {
	VertexBufferLayout::VertexBufferLayout(
		unsigned instanceStride)
		: m_stride(0)
		, m_instanceStride(instanceStride)
	{}

	template<>
	void VertexBufferLayout::Push<float>(
		unsigned count)
	{
		m_elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_stride += count * GetSizeOfType(GL_FLOAT);
	}

	template<>
	void VertexBufferLayout::Push<unsigned>(
		unsigned count)
	{
		m_elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_stride += count * GetSizeOfType(GL_UNSIGNED_INT);
	}

	template<>
	void VertexBufferLayout::Push<unsigned char>(
		unsigned count)
	{
		m_elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
		m_stride += count * GetSizeOfType(GL_UNSIGNED_BYTE);
	}

	void VertexBufferLayout::Clear() {
		m_elements.clear();
		m_elements.shrink_to_fit();
	}

	const std::vector<VertexBufferLayoutElement>& VertexBufferLayout::GetElements() const {
		return m_elements;
	}

	unsigned VertexBufferLayout::GetStride() const {
		return m_stride;
	}

	unsigned int RenderAPI::GetSizeOfType(
		unsigned int glType)
	{
		switch (glType) {
			case GL_FLOAT:         return sizeof(GLfloat);
			case GL_UNSIGNED_INT:  return sizeof(GLuint);
			case GL_UNSIGNED_BYTE: return sizeof(GLbyte);
		}

		return 0;
	}
}
}
