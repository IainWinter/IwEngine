#pragma once

#include "Mesh.h"

namespace IW {
inline namespace Graphics {
	struct Model {
		Mesh* Meshes;
		size_t MeshCount;
	};
}
}
