#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/SphereCollider.h"

namespace IW {
inline namespace Physics {
inline namespace Algo {
	template<
		typename V>
	bool TestCollision(
		const SphereCollider<V> &left,
		const SphereCollider<V> &right,
		V *resolve)
	{
		double r = left.Radius + right.Radius;
		V d = left.Center - right.Center;
		if (d.length() < r) {
			return r - d;
		}
	}



}
}
}
