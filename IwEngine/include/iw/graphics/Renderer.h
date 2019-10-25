#pragma once

#include "IwGraphics.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/smart_pointers.h"
#include "Asset/ModelData.h"

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API Renderer {
	public:
		iwu::ref<IW::IDevice> Device;

	public:
		Renderer(
			const iwu::ref<IW::IDevice>& device);

		//void BeginScene(Camera*);
		//void EndScene();

		//void DrawMesh();

	};
}
}
