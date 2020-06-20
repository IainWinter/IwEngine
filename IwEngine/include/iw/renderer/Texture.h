#pragma once

#include "IwRenderer.h"

namespace iw {
namespace RenderAPI {
	class ITexture {
	public:
		IWRENDERER_API
		virtual ~ITexture() = default;

		IWRENDERER_API
		virtual void GenerateMipMaps() const = 0;

		IWRENDERER_API
		virtual void Clear(
			const void* color) const = 0;

		IWRENDERER_API
		virtual void SetBorderColor(
			const void* color) const = 0;

		IWRENDERER_API virtual void SetFilter      (TextureFilter       filter)       = 0;
		IWRENDERER_API virtual void SetMipmapFilter(TextureMipmapFilter mipmapFilter) = 0;

		IWRENDERER_API
		virtual unsigned Id() const = 0;
	protected:
		ITexture() = default;
	};
}

	using namespace RenderAPI;
}
