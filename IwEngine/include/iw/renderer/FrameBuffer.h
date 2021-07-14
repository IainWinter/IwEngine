#pragma once

#include "IwRenderer.h"
#include "Texture.h"
#include <vector>

namespace iw {
namespace RenderAPI {
	// trying new api, device should only be for creation / settings
	// these interfaces are fine to use for function calls

	class IFrameBuffer {
	public:
		virtual ~IFrameBuffer() = default;

		virtual bool AttachTexture(
			ITexture* texture) = 0;

		virtual void SetDrawable(
			const std::vector<unsigned>& colorAttachments) = 0;

		virtual void ReadPixels() = 0;
		virtual void WritePixels() = 0;
		virtual void Bind()   const = 0;
		virtual void Unbind() const = 0;

	protected:
		IFrameBuffer() {}
	};
}

	using namespace RenderAPI;
}
