#pragma once

#include "iw/graphics/IwGraphics.h"
#include "iw/math/matrix4.h"
#include "iw/common/Components/Transform.h"

namespace IW {
inline namespace Graphics {
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
}
