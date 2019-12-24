#include "iw/common/Components/Transform.h"

namespace IW {
	iw::matrix4 Transform::Transformation() const {
		return iw::matrix4::create_scale(Scale)
			 * iw::matrix4::create_translation(Position)
			 * iw::matrix4::create_from_quaternion(Rotation);
	}

	iw::vector3 Transform::Forward() const {
		return iw::vector3::unit_z * Rotation;
	}

	iw::vector3 Transform::Right() const {
		return iw::vector3::unit_x * Rotation;
	}

	iw::vector3 Transform::Up() const {
		return iw::vector3::unit_y * Rotation;
	}
}
