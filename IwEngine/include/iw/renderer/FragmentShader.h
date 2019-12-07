#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API IFragmentShader {
	public:
		virtual ~IFragmentShader() {}
	protected:
		IFragmentShader() {}
	};
}

	using namespace RenderAPI;
}
