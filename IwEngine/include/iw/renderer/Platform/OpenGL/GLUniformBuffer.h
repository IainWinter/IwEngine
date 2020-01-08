#pragma once

#include "iw/renderer/UniformBuffer.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API GLUniformBuffer
		: public IUniformBuffer
	{
	private:
		unsigned m_renderId;
		size_t m_size;
		const void* m_data;

	public:
		GLUniformBuffer(
			size_t size,
			const void* data);

		~GLUniformBuffer();

		void UpdateData(
			const void* data) const;

		void BindBase(
			unsigned index) const;

		void Bind() const;
		void Unbind() const;
	};
}

	using namespace RenderAPI;
}
