#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/common/Components/Transform.h"

namespace iw {
namespace Graphics {
	struct VertexWeight {
		unsigned int Index;
		float Weight;
	};

	struct Bone {
		Transform Offset;

		VertexWeight* Weights;
		size_t WeightCount;
	};
}

	using namespace Graphics;
}
