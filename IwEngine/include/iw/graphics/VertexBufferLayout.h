#pragma once

#include <vector>
#include "GL\glew.h"

namespace IwGraphics {
	struct VertexBufferLayoutElement {
		unsigned int Type;
		unsigned int Count;
		unsigned char Normalized;

		static unsigned int GetSizeOfType(
			unsigned int glType);
	};

	class VertexBufferLayout {
	private:
		std::vector<VertexBufferLayoutElement> m_elements;
		unsigned int m_stride;

	public:
		VertexBufferLayout();

		template<
			typename _t>
		void Push(
			unsigned int count)
		{
			static_assert(false);
		}

		template<>
		void Push<float>(
			unsigned int count)
		{
			m_elements.push_back({ GL_FLOAT, count, GL_FALSE });
			m_stride += count * VertexBufferLayoutElement
				::GetSizeOfType(GL_FLOAT);
		}

		template<>
		void Push<unsigned int>(
			unsigned int count)
		{
			m_elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
			m_stride += count * VertexBufferLayoutElement
				::GetSizeOfType(GL_UNSIGNED_INT);
		}

		template<>
		void Push<unsigned char>(
			unsigned int count)
		{
			m_elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
			m_stride += count * VertexBufferLayoutElement
				::GetSizeOfType(GL_UNSIGNED_BYTE);
		}

		inline const
		std::vector<VertexBufferLayoutElement>& GetElements() const {
			return m_elements;
		}

		inline unsigned int GetStride() const {
			return m_stride;
		}
	};
}
