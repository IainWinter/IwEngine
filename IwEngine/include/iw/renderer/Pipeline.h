#pragma once

#include "PipelineParam.h"
#include "UniformBuffer.h"
#include <string>

namespace iw {
namespace RenderAPI {
	class IPipeline {
	public:
		IWRENDERER_API
			virtual ~IPipeline() = default;

		IWRENDERER_API
		virtual int UniformCount() = 0;

		IWRENDERER_API
		virtual IPipelineParam* GetParam(
			std::string name) = 0;

		IWRENDERER_API
		virtual IPipelineParam* GetParam(
			int index) = 0;

		IWRENDERER_API
		virtual void SetBuffer(
			std::string name,
			IUniformBuffer* buffer) = 0;
	};
}

	using namespace RenderAPI;
}
