#pragma once

#include "iw/common/iwcommon.h"
#include "iw/math/vector3.h"
#include "iw/math/quaternion.h"
#include "iw/math/matrix4.h"

namespace IW {
inline namespace Engine {
	struct IWCOMMON_API Transform {
		iw::vector3    Position = iw::vector3::zero;
		iw::vector3    Scale    = iw::vector3::one;
		iw::quaternion Rotation = iw::quaternion::identity;

		iw::matrix4 Transformation() const;
		iw::vector3 Forward() const;
		iw::vector3 Right() const;
		iw::vector3 Up() const;
	};
}
}
