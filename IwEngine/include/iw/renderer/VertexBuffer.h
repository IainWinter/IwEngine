#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API IVertexBuffer {
	public:
		virtual ~IVertexBuffer() {}
	protected:
		IVertexBuffer() {}
	};
}

	using namespace RenderAPI;
}
