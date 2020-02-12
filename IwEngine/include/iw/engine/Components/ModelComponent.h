#pragma once

#include "iw/engine/Core.h"
#include "iw/graphics/Mesh.h"
#include "iw/graphics/Model.h"

namespace iw {
	struct Model {
		Mesh*  Meshes;
		size_t MeshCount;
	};
}
