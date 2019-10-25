#pragma once

#include "IwRenderer.h"

namespace IW {
inline namespace Renderer {
	class IWRENDERER_API IFragmentShader {
	public:
		virtual ~IFragmentShader() {}
	protected:
		IFragmentShader() {}
	};
}
}
