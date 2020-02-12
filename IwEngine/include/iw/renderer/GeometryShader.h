#pragma once

#include "IwRenderer.h"

namespace iw {
namespace RenderAPI {
	class IGeometryShader {
	public:
		IWRENDERER_API
		virtual ~IGeometryShader() = default;
	protected:
		IGeometryShader() = default;
	};
}

	using namespace RenderAPI;
}
