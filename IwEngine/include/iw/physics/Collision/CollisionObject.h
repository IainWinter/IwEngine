#pragma once

#include "iw/common/Components/Transform.h"
#include "iw/physics/Collision/Collider.h"
#include <functional>

namespace iw {
namespace Physics {
	struct Manifold;

	using func_CollisionCallback = std::function<void(Manifold&, scalar)>;

	class CollisionObject {
	public:
		Transform Transform;
		Collider* Collider;

		bool IsTrigger;
		bool IsStatic;
		const bool IsDynamic;

		func_CollisionCallback OnCollision;

	public:
		CollisionObject(
			bool __isDynamic = false
		)
			: Collider(nullptr)
			, IsTrigger(false)
			, IsStatic(true)
			, IsDynamic(__isDynamic)
		{
			// maybe put check for if dynamic flag matches this ptr
		}

		virtual void SetTransform(
			iw::Transform* transform)
		{
			Transform = *transform;
		}
	};
}

	using namespace Physics;
}
