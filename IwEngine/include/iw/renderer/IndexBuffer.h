#pragma once

#include "IwRenderer.h"

namespace IwRenderer {
	class IWRENDERER_API IIndexBuffer {
	public:
		virtual ~IIndexBuffer() {}
	protected:
		IIndexBuffer() {}
	};
}
