#pragma once

#include "Pipeline.h"

namespace IW {
inline namespace Renderer {
	class IWRENDERER_API IComputePipeline {
	public:
		virtual ~IComputePipeline() {}

		virtual void DispatchComputeShader(
			int x,
			int y,
			int z) = 0;
	};
}
}
