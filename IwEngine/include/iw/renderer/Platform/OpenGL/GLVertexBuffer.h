#pragma once

#include "iw/renderer/IwRenderer.h"
#include "iw/renderer/VertexBuffer.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API GLVertexBuffer
		: public IVertexBuffer
	{
	private:
		unsigned m_renderId;
		size_t m_size;
		const void* m_data;

	public:
		GLVertexBuffer(
			size_t size,
			const void* data);

		~GLVertexBuffer();

		void UpdateData(
			size_t size,
			const void* data) const;

		void Bind() const;
		void Unbind() const;
	};
}

	using namespace RenderAPI;
}
