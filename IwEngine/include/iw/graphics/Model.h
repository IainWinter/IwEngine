#pragma once

#include "Mesh.h"

namespace IW {
namespace Graphics {
	struct Model {
		Mesh*  Meshes;
		size_t MeshCount;
	};
}

	using namespace Graphics;
}
