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
		virtual int UniformCount() const = 0;

		IWRENDERER_API
		virtual int BufferCount() const = 0;

		IWRENDERER_API
		virtual unsigned Handle() const = 0;

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

		//IWRENDERER_API
		//virtual bool IsTextureActive(
		//	IPipelineParam* param) const = 0;

		//IWRENDERER_API
		//virtual bool IsActive(
		//	IUniformBuffer* buffer) const = 0;
	};
}

	using namespace RenderAPI;
}
