#pragma once

#include "PipelineParam.h"
#include "UniformBuffer.h"
#include "iw/math/matrix4.h"

namespace IW {
namespace RenderAPI {
	class IPipeline {
	public:
		IWRENDERER_API
		virtual ~IPipeline() {}

		IWRENDERER_API
		virtual IPipelineParam* GetParam(
			const char* name) = 0;

		IWRENDERER_API
		virtual IPipelineParam* GetParam(
			int index) = 0;

		IWRENDERER_API
		virtual void SetBuffer(
			const char* name,
			IUniformBuffer* buffer) = 0;

		IWRENDERER_API
		virtual int UniformCount() = 0;
	};
}

	using namespace RenderAPI;
}
