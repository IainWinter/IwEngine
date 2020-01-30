#pragma once

#include "Pipeline.h"

namespace IW {
namespace RenderAPI {
	class IComputePipeline {
	public:
		IWRENDERER_API
		virtual ~IComputePipeline() = default;

		IWRENDERER_API
		virtual void DispatchComputeShader(
			int x,
			int y,
			int z) = 0;
	};
}

	using namespace RenderAPI;
}
