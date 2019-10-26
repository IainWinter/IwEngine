#pragma once

#include "IwRenderer.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API IVertexShader {
	public:
		virtual ~IVertexShader() {}
	protected:
		IVertexShader() {}
	};
}
}
