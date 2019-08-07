#pragma once

#include "iw/data/iwdata.h"
#include "iw/math/vector3.h"
#include "iw/math/quaternion.h"
#include "iw/math/matrix4.h"

namespace IwEngine {
	struct IWDATA_API Transform {
		iwm::vector3    Position = iwm::vector3::zero;
		iwm::vector3    Scale    = iwm::vector3::one;
		iwm::quaternion Rotation = iwm::quaternion::identity;

		iwm::matrix4 Transformation();
		iwm::vector3 Forward();
		iwm::vector3 Right();
		iwm::vector3 Up();
	};
}
