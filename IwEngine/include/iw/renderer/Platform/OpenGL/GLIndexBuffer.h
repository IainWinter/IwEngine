#pragma once

#include "iw/renderer/IndexBuffer.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API GLIndexBuffer
		: public IIndexBuffer
	{
	private:
		unsigned int m_renderId;
		unsigned int m_count;
		const void* m_data;

	public:
		GLIndexBuffer(
			unsigned int count,
			const void* data);

		~GLIndexBuffer();

		void Bind()   const;
		void Unbind() const;

		inline unsigned int GetCount() const {
			return m_count;
		}
	};
}
}
