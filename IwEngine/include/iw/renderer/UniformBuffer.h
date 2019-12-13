#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API IUniformBuffer {
	public:
		virtual ~IUniformBuffer() {}
	protected:
		IUniformBuffer() {}
	};
}

	using namespace RenderAPI;
}
