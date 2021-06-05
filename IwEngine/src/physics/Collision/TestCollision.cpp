#pragma once

#include "iw/physics/Collision/TestCollision.h"
#include "iw/physics/impl/TestCollision.h"

namespace iw {
namespace Physics {
	ManifoldPoints TestCollision(
		const Collider* a, const Transform* at,
		const Collider* b, const Transform* bt)
	{
		static impl::Test_Collision_funcs<d2> d2;
		static impl::Test_Collision_funcs<d3> d3;

		if (a->Dim != b->Dim) return {};

		Dimension dim = a->Dim;

		size_t aid = a->get_id();
		size_t bid = b->get_id();

		if (aid > bid) {
			size_t tid = aid;
			aid = bid;
			bid = tid;

			const Collider* tc = a;
			a = b;
			b = tc;

			const Transform* tt = at;
			at = bt;
			bt = tt;
		}

		if (dim == Dimension::d2)
		{
			auto func = d2.tests[aid][bid];
			if (func) {
				return func(a, at, b, bt);
			}
		}

		else 
		if (dim == Dimension::d3)
		{
			auto func = d3.tests[aid][bid];
			if (func) {
				return func(a, at, b, bt);
			}
		}

		//assert(false);
		return ManifoldPoints();
	}
}

	using namespace Physics;
}
