#pragma once

#include "IwRenderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace IW {
namespace RenderAPI {
	class IVertexArray {
	public:
		IWRENDERER_API
		virtual ~IVertexArray() = default;
	protected:
		IVertexArray() = default;
	};
}

	using namespace RenderAPI;
}
