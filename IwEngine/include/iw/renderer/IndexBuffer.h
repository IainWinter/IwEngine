#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API IIndexBuffer {
	public:
		virtual ~IIndexBuffer() {}
	protected:
		IIndexBuffer() {}
	};
}

	using namespace RenderAPI;
}
