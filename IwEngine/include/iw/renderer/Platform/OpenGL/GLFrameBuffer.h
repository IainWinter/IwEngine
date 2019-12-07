#pragma once

#include "GLTexture.h"
#include "iw/renderer/FrameBuffer.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API GLFrameBuffer
		: public IFrameBuffer
	{
	private:
		unsigned int m_renderId;
		unsigned int m_textureCount;

	public:
		GLFrameBuffer();

		~GLFrameBuffer();

		void AttachTexture(
			GLTexture* texture);

		void Bind() const;
		void Unbind() const;
	};
}

	using namespace RenderAPI;
}
