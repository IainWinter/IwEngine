#pragma once

#include "PipelineParam.h"
#include "iw/math/matrix4.h"

namespace IwRenderer {
	class IWRENDERER_API IPipeline {
	public:
		virtual ~IPipeline() {}

		virtual IPipelineParam* GetParam(
			const char* name) = 0;
	};
}
