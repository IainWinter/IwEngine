#pragma once

#include "iw/engine/Core.h"
#include "iw/math/matrix4.h"

namespace IwEngine {
	struct Camera {
		iwm::matrix4 Projection;
	};
}
