#include "iw/engine/Components/Transform.h"

namespace IwEngine {
	iwm::matrix4 Transform::Transformation() {
		return iwm::matrix4::create_from_quaternion(Rotation)
			 * iwm::matrix4::create_scale(Scale)
			 * iwm::matrix4::create_translation(Position);
	}

	iwm::vector3 Transform::Forward() {
		return -iwm::vector3::unit_z * Rotation;
	}

	iwm::vector3 Transform::Right() {
		return iwm::vector3::unit_x * Rotation;
	}

	iwm::vector3 Transform::Up() {
		return iwm::vector3::unit_y * Rotation;
	}
}
