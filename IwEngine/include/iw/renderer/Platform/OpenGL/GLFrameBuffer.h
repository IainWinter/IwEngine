#pragma once

#include "GLTexture.h"
#include "iw/renderer/FrameBuffer.h"
#include <vector>

namespace iw {
namespace RenderAPI {
	class GLFrameBuffer
		: public IFrameBuffer
	{
	private:
		unsigned gl_id;
		unsigned m_textureCount;
		std::vector<std::pair<int, GLTexture*>> m_attachments;

	public:
		IWRENDERER_API
		GLFrameBuffer();

		IWRENDERER_API
		~GLFrameBuffer();

		IWRENDERER_API
		bool AttachTexture(
			ITexture* texture) override;

		IWRENDERER_API
		void SetDrawable(
			const std::vector<unsigned>& colorAttachments) override;

		IWRENDERER_API void ReadPixels() override;
		IWRENDERER_API void WritePixels() override;
		IWRENDERER_API void Bind()   const override;
		IWRENDERER_API void Unbind() const override;
	};
}

	using namespace RenderAPI;
}
