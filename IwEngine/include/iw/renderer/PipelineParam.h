#pragma once

#include "IwRenderer.h"
#include "iw/math/matrix4.h"

namespace IwRenderer {
	class IWRENDERER_API PipelineParam {
	public:
		virtual ~PipelineParam() {}

		virtual void SetAsMat4(
			const iwm::matrix4& matrix) = 0;
	};
}
