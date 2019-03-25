#pragma once

#include "IwRenderer.h"

namespace IwRenderer {
	class IWRENDERER_API IVertexArray {
	public:
		virtual ~IVertexArray() {}
	protected:
		IVertexArray() {}
	};
}
