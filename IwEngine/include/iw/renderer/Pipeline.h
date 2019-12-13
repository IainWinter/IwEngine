#pragma once

#include "PipelineParam.h"
#include "UniformBuffer.h"
#include "iw/math/matrix4.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API IPipeline {
	public:
		virtual ~IPipeline() {}

		virtual IPipelineParam* GetParam(
			const char* name) = 0;

		virtual void SetBuffer(
			const char* name,
			IUniformBuffer* buffer) = 0;
	};
}

	using namespace RenderAPI;
}
