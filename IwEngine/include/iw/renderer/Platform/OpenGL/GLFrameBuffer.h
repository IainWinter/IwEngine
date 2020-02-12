#pragma once

#include "GLTexture.h"
#include "iw/renderer/FrameBuffer.h"

namespace iw {
namespace RenderAPI {
	class GLFrameBuffer
		: public IFrameBuffer
	{
	private:
		unsigned int gl_id;
		unsigned int m_textureCount;

	public:
		IWRENDERER_API
		GLFrameBuffer();

		IWRENDERER_API
		~GLFrameBuffer();

		IWRENDERER_API
		void AttachTexture(
			GLTexture* texture);

		IWRENDERER_API
		void Bind() const;

		IWRENDERER_API
		void Unbind() const;
	};
}

	using namespace RenderAPI;
}
