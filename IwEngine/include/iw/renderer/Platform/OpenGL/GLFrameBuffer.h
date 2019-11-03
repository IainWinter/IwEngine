#pragma once

#include "GLTexture.h"
#include "iw/renderer/FrameBuffer.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API GLFrameBuffer
		: public IFrameBuffer
	{
	private:
		unsigned int m_renderId;

	public:
		GLFrameBuffer(
			ITexture* texture);

		~GLFrameBuffer();

		void Bind() const;
		void Unbind() const;
	};
}
}
