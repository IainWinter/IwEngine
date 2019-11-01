#pragma once

#include "iw/engine/Core.h"
#include "iw/graphics/Mesh.h"
#include "iw/graphics/Model.h"

namespace IwEngine {
	struct Model {
		IW::Mesh* Meshes;
		size_t    MeshCount;
	};
}
