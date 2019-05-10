#pragma once

#include "Pipeline.h"

namespace IwRenderer {
	class IWRENDERER_API IComputePipeline
		: IPipeline
	{
	public:
		virtual ~IComputePipeline() {}

		virtual void DispatchComputeShader(
			int x,
			int y,
			int z) = 0;
	};
}
