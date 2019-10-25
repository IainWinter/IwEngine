#pragma once

#include "PipelineParam.h"
#include "iw/math/matrix4.h"

namespace IW {
inline namespace Renderer {
	class IWRENDERER_API IPipeline {
	public:
		virtual ~IPipeline() {}

		virtual IPipelineParam* GetParam(
			const char* name) = 0;

		//virtual IPipelineParam* GetBuffer(
		//	const char* name) = 0;
	};
}
}
