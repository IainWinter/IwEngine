#pragma once

#include "IwRenderer.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API IIndexBuffer {
	public:
		virtual ~IIndexBuffer() {}
	protected:
		IIndexBuffer() {}
	};
}
}
