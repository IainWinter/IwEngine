#pragma once

#include "IwRenderer.h"

namespace IwRenderer {
	class IWRENDERER_API IFragmentShader {
	public:
		virtual ~IFragmentShader() {}
	protected:
		IFragmentShader() {}
	};
}
