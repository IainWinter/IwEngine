#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API ITexture {
	public:
		virtual ~ITexture() {}

	protected:
		ITexture() {}
	};
}

	using namespace RenderAPI;
}
