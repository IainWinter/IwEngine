#pragma once

#include "IwGraphics.h"
#include <vector>

namespace IwGraphics {
	struct IWGRAPHICS_API VertexBufferLayoutElement {
		unsigned int Type;
		unsigned int Count;
		unsigned char Normalized;

		static unsigned int GetSizeOfType(
			unsigned int glType);
	};

	class IWGRAPHICS_API VertexBufferLayout {
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
			unsigned int count);

		template<>
		void Push<unsigned int>(
			unsigned int count);

		template<>
		void Push<unsigned char>(
			unsigned int count);

		inline const
		std::vector<VertexBufferLayoutElement>& GetElements() const {
			return m_elements;
		}

		inline unsigned int GetStride() const {
			return m_stride;
		}
	};
}
