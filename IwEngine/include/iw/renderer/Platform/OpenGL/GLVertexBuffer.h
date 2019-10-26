#pragma once

#include "iw/renderer/IwRenderer.h"
#include "iw/renderer/VertexBuffer.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API GLVertexBuffer
		: public IVertexBuffer
	{
	private:
		unsigned int m_renderId;
		unsigned int m_size;
		const void* m_data;

	public:
		GLVertexBuffer(
			unsigned int size,
			const void* data);

		~GLVertexBuffer();

		void UpdateData(
			unsigned int size,
			const void* data) const;

		void Bind() const;
		void Unbind() const;
	};
}
}
