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
			float r,
			float g,
			float b,
			float a = 1.0f) const = 0;
	protected:
		ITexture() = default;
	};
}

	using namespace RenderAPI;
}
