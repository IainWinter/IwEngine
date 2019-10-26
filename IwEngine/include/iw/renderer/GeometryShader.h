#pragma once

#include "IwRenderer.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API IGeometryShader {
	public:
		virtual ~IGeometryShader() {}
	protected:
		IGeometryShader() {}
	};
}
}
