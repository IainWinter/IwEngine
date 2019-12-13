#pragma once

#include "IwRenderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API IVertexArray {
	public:
		virtual ~IVertexArray() {}

	protected:
		IVertexArray() {}
	};
}

	using namespace RenderAPI;
}
