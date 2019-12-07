#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API IVertexShader {
	public:
		virtual ~IVertexShader() {}
	protected:
		IVertexShader() {}
	};
}

	using namespace RenderAPI;
}
