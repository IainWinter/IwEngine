#pragma once

#include "IwRenderer.h"

namespace IwRenderer {
	class IWRENDERER_API IGeometryShader {
	public:
		virtual ~IGeometryShader() {}
	protected:
		IGeometryShader() {}
	};
}
