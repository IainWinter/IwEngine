#pragma once

#include "IwRenderer.h"

namespace IW {
inline namespace Renderer {
	class IWRENDERER_API IComputeShader {
	public:
		virtual ~IComputeShader() {}
	protected:
		IComputeShader() {}
	};
}
}
