#pragma once

#include "IwRenderer.h"

namespace iw {
namespace RenderAPI {
	class ITexture {
	public:
		IWRENDERER_API
		virtual ~ITexture() = default;
	protected:
		ITexture() = default;
	};
}

	using namespace RenderAPI;
}
