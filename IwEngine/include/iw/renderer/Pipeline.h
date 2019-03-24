#pragma once

#include "PipelineParam.h"
#include "iw/math/matrix4.h"

namespace IwRenderer {
	class IWRENDERER_API Pipeline {
	public:
		virtual ~Pipeline() {}

		virtual PipelineParam* GetParam(
			const char* name) = 0;
	};
}
