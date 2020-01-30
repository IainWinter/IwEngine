#pragma once

#include "iw/renderer/IwRenderer.h"
#include "iw/renderer/Buffer.h"

namespace IW {
namespace RenderAPI {
	class GLBuffer
		: public virtual IBuffer
	{
	private:
		unsigned gl_id;
		int gl_type;
		int gl_io;

		BufferType   m_type;
		BufferIOType m_io;

		const void*  m_data;
		size_t       m_size;

	public:
		IWRENDERER_API
		void UpdateData(
			const void* data,
			size_t size,
			size_t offset = 0);

		IWRENDERER_API
		void Bind() const;

		IWRENDERER_API
		void BindBase(
			unsigned index) const;

		IWRENDERER_API
		void Unbind() const;

		IWRENDERER_API
		unsigned Id() const;
	protected:
		GLBuffer(
			BufferType type,
			BufferIOType io,
			const void* data,
			size_t size);

		virtual ~GLBuffer();
	private:
		GLBuffer(const GLBuffer&) = delete;
	};
}

	using namespace RenderAPI;
}
