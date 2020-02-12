#pragma once

#include "Mesh.h"

namespace iw {
namespace Graphics {
	struct Model {
		Mesh*  Meshes;
		size_t MeshCount;
	};
}

	using namespace Graphics;
}
