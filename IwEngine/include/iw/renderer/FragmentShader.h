#pragma once

#include "IwRenderer.h"

namespace iw {
namespace RenderAPI {
	class IFragmentShader {
	public:
		IWRENDERER_API
		virtual ~IFragmentShader() = default;
	protected:
		IFragmentShader() {}
	};
}

	using namespace RenderAPI;
}
