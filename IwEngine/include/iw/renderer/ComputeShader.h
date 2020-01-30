#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	class IComputeShader {
	public:
		IWRENDERER_API
		virtual ~IComputeShader() = default;
	protected:
		IComputeShader() = default;
	};
}

	using namespace RenderAPI;
}
