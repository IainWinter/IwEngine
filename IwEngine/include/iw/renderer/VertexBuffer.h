#pragma once

#include "IwRenderer.h"
#include "Buffer.h"

namespace IW {
namespace RenderAPI {
	class IVertexBuffer
		: public virtual IBuffer
	{
	public:
		IWRENDERER_API
		virtual ~IVertexBuffer() = default;
	protected:
		IVertexBuffer() = default;
	};
}

	using namespace RenderAPI;
}
