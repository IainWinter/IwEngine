#pragma once

#include "IwRenderer.h"

namespace IwRenderer {
	class IWRENDERER_API FragmentShader {
	public:
		virtual ~FragmentShader() {}
	protected:
		FragmentShader() {}
	};
}
