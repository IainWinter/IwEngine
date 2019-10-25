#pragma once

#include "iw/engine/Core.h"
#include "iw/graphics/Mesh.h"
#include "iw/graphics/Asset/ModelData.h"

namespace IwEngine {
	struct Model {
		IW::Mesh* Meshes;
		size_t    MeshCount;
	};
}
