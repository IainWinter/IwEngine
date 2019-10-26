#include "iw/data/Components/Transform.h"

namespace IW {
	iwm::matrix4 Transform::Transformation() const {
		return iwm::matrix4::create_from_quaternion(Rotation)
			 * iwm::matrix4::create_scale(Scale)
			 * iwm::matrix4::create_translation(Position);
	}

	iwm::vector3 Transform::Forward() const {
		return -iwm::vector3::unit_z * Rotation;
	}

	iwm::vector3 Transform::Right() const {
		return iwm::vector3::unit_x * Rotation;
	}

	iwm::vector3 Transform::Up() const {
		return iwm::vector3::unit_y * Rotation;
	}
}
