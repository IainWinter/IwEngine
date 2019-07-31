#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/Asset/ModelData.h"
#include "iw/renderer/Device.h"
#include "Mesh.h"

namespace IwEngine {
	struct IWENGINE_API Model {
		ModelData*  Data;
		Mesh*       Meshes;
		std::size_t MeshCount;

		Model(
			ModelData* data,
			IwRenderer::IDevice* device);

		Model(
			Model&& copy);

		Model(
			const Model& copy);

		~Model();

		Model& operator=(
			Model&& copy);

		Model& operator=(
			const Model& copy);
	};
}
