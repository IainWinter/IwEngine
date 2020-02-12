#pragma once

#include "IwRenderer.h"

namespace iw {
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
