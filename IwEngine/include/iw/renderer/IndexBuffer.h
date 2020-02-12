#pragma once

#include "IwRenderer.h"
#include "Buffer.h"

namespace iw {
namespace RenderAPI {
	class IIndexBuffer
		: public virtual IBuffer
	{
	public:
		IWRENDERER_API
		virtual ~IIndexBuffer() = default;
	protected:
		IIndexBuffer() = default;
	};
}

	using namespace RenderAPI;
}
