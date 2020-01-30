#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	class IBuffer {
	public:
		IWRENDERER_API
		virtual ~IBuffer() = default;

	protected:
		IBuffer() = default;
	};
}

	using namespace RenderAPI;
}
