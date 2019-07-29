#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/Asset/ModelData.h"
#include "iw/renderer/Device.h"
#include "Mesh.h"

namespace IwEngine {
	class IWENGINE_API Model {
	public:
		ModelData*  Data;
		Mesh*       Meshes;
		std::size_t MeshCount;

		Model() = default;

		Model(
			ModelData* data,
			IwRenderer::IDevice* device);
	};
}
