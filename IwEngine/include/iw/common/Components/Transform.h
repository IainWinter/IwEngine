#pragma once

#include "iw/common/iwcommon.h"
#include "iw/math/quaternion.h"
#include "iw/math/vector3.h"
#include "iw/math/matrix4.h"
#include <vector>

namespace iw {
namespace Engine {
	struct Transform {
		vector3 Position;
		vector3 Scale;
		quaternion Rotation;
	private:
		std::vector<Transform*> m_children;
		Transform* m_parent;

	public:
		IWCOMMON_API
		Transform();

		IWCOMMON_API
		Transform(
			vector3    position,
			vector3    scale    = vector3::one,
			quaternion rotation = quaternion::identity);

		IWCOMMON_API
		matrix4 Transformation() const;

		IWCOMMON_API
		vector3 Forward() const;

		IWCOMMON_API
		vector3 Right() const;

		IWCOMMON_API
		vector3 Up() const;

		IWCOMMON_API
		size_t ChildCount() const;

		IWCOMMON_API std::vector<Transform*>&       Children();
		IWCOMMON_API const std::vector<Transform*>& Children() const;

		IWCOMMON_API
		void AddChild(
			Transform* transform);

		IWCOMMON_API
		void RemoveChild(
			Transform* transform);

		IWCOMMON_API       Transform* Parent();
		IWCOMMON_API const Transform* Parent() const;

		IWCOMMON_API
		void SetParent(
			Transform* transform);
	};
}

	using namespace Engine;
}
