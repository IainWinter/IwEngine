#pragma once

#include "GLVertexBuffer.h"
#include "iw/renderer/IwRenderer.h"
#include "iw/renderer/VertexArray.h"
#include "iw/renderer/VertexBufferLayout.h"
#include <vector>

namespace IwRenderer {
	class IWRENDERER_API GLVertexArray
		: public IVertexArray
	{
	private:
		unsigned int m_renderId;
		std::vector<GLVertexBuffer*> m_buffers;
		std::vector<VertexBufferLayout*> m_layouts;

	public:
		GLVertexArray();
		~GLVertexArray();

		void AddBuffer(
			GLVertexBuffer* vb,
			VertexBufferLayout* layout);

		void Bind() const;
		void Unbind() const;
	};
}
