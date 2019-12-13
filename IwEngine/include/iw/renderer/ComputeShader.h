#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API IComputeShader {
	public:
		virtual ~IComputeShader() {}
	protected:
		IComputeShader() {}
	};
}

	using namespace RenderAPI;
}
