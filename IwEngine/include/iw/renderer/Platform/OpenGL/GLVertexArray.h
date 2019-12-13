#pragma once

#include "GLVertexBuffer.h"
#include "iw/renderer/VertexArray.h"
#include "iw/renderer/VertexBufferLayout.h"
#include <vector>

namespace IW {
namespace RenderAPI {
	class GLVertexArray
		: public IVertexArray
	{
	private:
		std::vector<GLVertexBuffer*>    m_buffers;
		std::vector<VertexBufferLayout> m_layouts;
		unsigned int m_renderId;

	public:
		GLVertexArray();
		~GLVertexArray();

		void AddBuffer(
			GLVertexBuffer* vb,
			const VertexBufferLayout& layout);

		void UpdateBuffer(
			size_t index,
			unsigned size,
			const void* data);

		void Bind() const;
		void Unbind() const;
	};
}

	using namespace RenderAPI;
}
