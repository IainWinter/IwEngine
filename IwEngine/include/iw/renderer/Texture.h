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
	protected:
		ITexture() = default;
	};
}

	using namespace RenderAPI;
}
