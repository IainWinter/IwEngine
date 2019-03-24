#pragma once

#include "IwRenderer.h"

namespace IwRenderer {
	class IWRENDERER_API VertexShader {
	public:
		virtual ~VertexShader() {}
	protected:
		VertexShader() {}
	};
}
