#pragma once

#include "iw/common/iwcommon.h"
#include "iw/reflection/Reflect.h"
#include "iw/math/vector3.h"
#include "iw/math/matrix4.h"
#include "iw/math/quaternion.h"
#include <vector>

namespace iw {
namespace Engine {
	struct REFLECT Transform {
		REFLECT vector3 Position;
		REFLECT vector3 Scale;
		REFLECT quaternion Rotation;
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

		IWCOMMON_API matrix4 Transformation() const;
		IWCOMMON_API vector3 Forward()        const;
		IWCOMMON_API vector3 Right()          const;
		IWCOMMON_API vector3 Up()             const;

		IWCOMMON_API matrix4    WorldTransformation() const;
		IWCOMMON_API vector3    WorldForward()        const;
		IWCOMMON_API vector3    WorldRight()          const;
		IWCOMMON_API vector3    WorldUp()             const;
		IWCOMMON_API vector3    WorldPosition()       const;
		IWCOMMON_API vector3    WorldScale()          const;
		IWCOMMON_API quaternion WorldRotation()       const;

		IWCOMMON_API
		size_t ChildCount() const;

		IWCOMMON_API       std::vector<Transform*>& Children();
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
