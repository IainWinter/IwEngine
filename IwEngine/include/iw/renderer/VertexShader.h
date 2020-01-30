#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	class IVertexShader {
	public:
		IWRENDERER_API
		virtual ~IVertexShader() = default;
	protected:
		IVertexShader() = default;
	};
}

	using namespace RenderAPI;
}
