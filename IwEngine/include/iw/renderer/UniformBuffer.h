#pragma once

#include "IwRenderer.h"
#include "Buffer.h"

namespace IW {
namespace RenderAPI {
	class IUniformBuffer
		: public virtual IBuffer
	{
	public:
		IWRENDERER_API
		virtual ~IUniformBuffer() = default;
	protected:
		IUniformBuffer() = default;
	};
}

	using namespace RenderAPI;
}
