#pragma once

#include "IwGraphics.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/smart_pointers.h"
#include "Mesh.h"
#include "Asset/ModelData.h"

namespace IwGraphics {
	class IWGRAPHICS_API Renderer {
	public:
		iwu::ref<IwRenderer::IDevice> Device;

	public:
		Renderer(
			const iwu::ref<IwRenderer::IDevice>& device);

		Mesh* CreateMesh(const MeshData& meshData);
	};
}
