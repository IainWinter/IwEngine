#pragma once

#include "iw/renderer/UniformBuffer.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API GLUniformBuffer
		: public IUniformBuffer
	{
	private:
		unsigned int m_renderId;
		const void* m_buffer;

		unsigned int m_size;
		const void* m_data;

	public:
		GLUniformBuffer(
			unsigned int size,
			const void* data);

		~GLUniformBuffer();

		void UpdateData(
			const void* data) const;

		void BindBase(
			unsigned int index) const;

		void Bind() const;
		void Unbind() const;
	};
}

	using namespace RenderAPI;
}
