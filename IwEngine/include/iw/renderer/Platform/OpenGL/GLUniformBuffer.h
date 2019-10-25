#pragma once

#include "iw/renderer/UniformBuffer.h"

namespace IW {
inline namespace Renderer {
	class IWRENDERER_API GLUniformBuffer
		: public IUniformBuffer
	{
	private:
		unsigned int m_renderId;
		unsigned int m_size;
		const void* m_data;

	public:
		GLUniformBuffer(
			unsigned int size,
			const void* data);

		~GLUniformBuffer();

		void UpdateData(
			unsigned int size,
			const void* data) const;

		void Bind() const;
		void Unbind() const;
	};
}
}
