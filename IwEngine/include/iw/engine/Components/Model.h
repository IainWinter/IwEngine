#pragma once

#include "iw/engine/Core.h"
#include "iw/renderer/Device.h"
#include "iw/graphics/RenderQueue.h"
#include "iw/graphics/Mesh.h"
#include "iw/graphics/Asset/ModelData.h"

namespace IwEngine {
	struct IWENGINE_API Model {
		IwGraphics::ModelData* Data;
		IwGraphics::Mesh*      Meshes;
		std::size_t            MeshCount;
		bool                   Initialized;

		Model(
			IwGraphics::ModelData* data,
			IwGraphics::RenderQueue& RenderQueue);

		Model(
			Model&& copy) noexcept;

		Model(
			const Model& copy);

		~Model();

		Model& operator=(
			Model&& copy) noexcept;

		Model& operator=(
			const Model& copy);
	};
}
