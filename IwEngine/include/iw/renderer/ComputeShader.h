#pragma once

#include "IwRenderer.h"

namespace IwRenderer {
	class IWRENDERER_API IComputeShader {
	public:
		virtual ~IComputeShader() {}
	protected:
		IComputeShader() {}
	};
}
