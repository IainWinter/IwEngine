#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/common/Components/Transform.h"

namespace IW {
namespace Physics {
	class CollisionObject {
	private:
		Transform m_transform;

	public:
		const Transform& Trans() const;

		IWPHYSICS_API
		void SetTrans(
			const Transform& transform);

		// aabb
		// transform
	};
}

	using namespace Physics;
}
