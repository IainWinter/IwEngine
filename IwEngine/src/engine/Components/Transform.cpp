#include "iw/engine/Components/Transform.h"

namespace IwEngine {
	Transform::Transform(
		iwm::vector3 position,
		iwm::vector3 scale,
		iwm::quaternion rotation)
		: Position(position)
		, Scale(scale)
		, Rotation(rotation)
	{}

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
