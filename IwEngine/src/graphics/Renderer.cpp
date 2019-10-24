#include "iw/graphics/Renderer.h"

namespace IwGraphics {
	Renderer::Renderer(
		const iwu::ref<IwRenderer::IDevice>& device)
		: Device(device)
	{}
}
