#pragma once

#include "iw/graphics/Mesh.h"
#include <vector>

namespace IwGraphics {
	struct Model {
		std::vector<Mesh> Meshes;
		//std::vector<Material> Materials;
	};
}
