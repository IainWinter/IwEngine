#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API IGeometryShader {
	public:
		virtual ~IGeometryShader() {}
	protected:
		IGeometryShader() {}
	};
}

	using namespace RenderAPI;
}
