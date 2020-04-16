#pragma once

#include "GLVertexBuffer.h"
#include "iw/renderer/VertexArray.h"
#include "iw/renderer/VertexBufferLayout.h"
#include <vector>

namespace iw {
namespace RenderAPI {
	class GLVertexArray
		: public IVertexArray
	{
	private:
		unsigned gl_id;

		std::vector<GLVertexBuffer*>    m_buffers;
		std::vector<VertexBufferLayout> m_layouts;

	public:
		IWRENDERER_API
		GLVertexArray();

		IWRENDERER_API
		~GLVertexArray();

		IWRENDERER_API
		void AddBuffer(
			GLVertexBuffer* vb,
			const VertexBufferLayout& layout,
			int index = -1); // if index is negitive the next slot gets used

		IWRENDERER_API
		void UpdateBuffer(
			size_t index,
			const void* data,
			size_t size,
			size_t offset = 0);

		IWRENDERER_API
		void Bind() const;

		IWRENDERER_API
		void Unbind() const;
	};
}

	using namespace RenderAPI;
}
