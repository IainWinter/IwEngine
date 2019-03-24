#pragma once

#include "iw/renderer/PipelineParam.h"

namespace IwRenderer {
	class IWRENDERER_API GLPipelineParam
		: public PipelineParam
	{
	private:
		unsigned int m_location;
	public:
		GLPipelineParam(int location);

		void SetAsMat4(
			const iwm::matrix4& matrix);
	};
}
