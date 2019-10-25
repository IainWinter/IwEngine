#pragma once

#include "IwRenderer.h"
#include "iw/log/logger.h"
#include <vector>

namespace IW {
inline namespace Renderer {
	struct VertexBufferLayoutElement {
		unsigned int  Type;
		unsigned int  Count;
		unsigned char Normalized;
	};

	class IWRENDERER_API VertexBufferLayout {
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
			LOG_ERROR << "Cannot push invalid buffer type!";
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

		void Clear() {
			m_elements.clear();
			m_elements.shrink_to_fit();
		}
	};

	unsigned int GetSizeOfType(
		unsigned int glType);
}
}
