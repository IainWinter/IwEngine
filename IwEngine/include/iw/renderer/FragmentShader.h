#pragma once

#include "IwRenderer.h"

namespace IW {
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
