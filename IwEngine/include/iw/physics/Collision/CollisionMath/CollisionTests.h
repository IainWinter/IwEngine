#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/SphereCollider.h"

namespace IW {
inline namespace Physics {
inline namespace Algo {
	//template<
	//	typename V>
	//bool TestCollision(
	//	const SphereCollider &left,
	//	const SphereCollider &right,
	//	V *resolve = nullptr)
	//{
	//	double r = left.Radius + right.Radius;
	//	V d = left.Center - right.Center;
	//	if (d.length() < r) {
	//		if(resolve != nullptr) *resolve = (d / d.length()) * (r - d.length());
	//		return true;
	//	}
	//	return false;
	//}

	//Box2 - Box2
	bool TestCollision(
		const BoxCollider<iw::vector2>& left,
		const BoxCollider<iw::vector2>& right,
		iw::vector2 *resolve = nullptr)
	{
		
	}
}
}

	using namespace Physics;
}
