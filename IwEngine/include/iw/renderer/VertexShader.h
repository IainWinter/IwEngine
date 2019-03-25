#pragma once

#include "IwRenderer.h"

namespace IwRenderer {
	class IWRENDERER_API IVertexShader {
	public:
		virtual ~IVertexShader() {}
	protected:
		IVertexShader() {}
	};
}
