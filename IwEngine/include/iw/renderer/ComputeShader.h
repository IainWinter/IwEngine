#pragma once

#include "IwRenderer.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API IComputeShader {
	public:
		virtual ~IComputeShader() {}
	protected:
		IComputeShader() {}
	};
}
}
