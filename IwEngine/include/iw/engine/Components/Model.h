#pragma once

#include "iw/engine/Core.h"
#include "iw/renderer/Mesh.h"
#include "iw/graphics/Asset/ModelData.h"

namespace IwEngine {
	struct Model {
		IwGraphics::ModelData* Data;
		IwRenderer::Mesh*      Meshes;
		size_t                 MeshCount;
	};
}
