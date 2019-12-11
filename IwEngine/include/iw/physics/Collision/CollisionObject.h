#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/common/Components/Transform.h"

namespace IW {
namespace Physics {
	class CollisionObject {
	protected:
		Transform m_transform;     // Position, rotation, and scale

	public:
		const Transform& Trans() const;

		IWPHYSICS_API
		void SetTrans(
			const Transform& transform);
	};
}

	using namespace Physics;
}
