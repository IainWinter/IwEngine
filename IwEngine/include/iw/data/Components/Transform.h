#pragma once

#include "iw/data/iwdata.h"
#include "iw/math/vector3.h"
#include "iw/math/quaternion.h"
#include "iw/math/matrix4.h"

namespace IW {
inline namespace Engine {
	struct IWDATA_API Transform {
		iwm::vector3    Position = iwm::vector3::zero;
		iwm::vector3    Scale    = iwm::vector3::one;
		iwm::quaternion Rotation = iwm::quaternion::identity;

		iwm::matrix4 Transformation() const;
		iwm::vector3 Forward() const;
		iwm::vector3 Right() const;
		iwm::vector3 Up() const;
	};
}
}
