#pragma once

#include "iw/physics/Collision/TestCollision.h"
#include "iw/physics/impl/TestCollision.h"

namespace iw {
namespace Physics {
	ManifoldPoints TestCollision(
		Collider* a, Transform* at,
		Collider* b, Transform* bt)
	{
		static impl::Test_Collision_funcs<d2> d2;
		static impl::Test_Collision_funcs<d3> d3;

		if (a->Dim != b->Dim) {
			LOG_WARNING << "Tried to collide 2d and 3d objects!";
			return {};
		}

		Dimension dim = a->Dim;

		size_t aid = a->get_id();
		size_t bid = b->get_id();

		bool swap = aid > bid;

		if (swap) {
			size_t tid = aid;
			aid = bid;
			bid = tid;

			Collider* tc = a;
			a = b;
			b = tc;

			Transform* tt = at;
			at = bt;
			bt = tt;
		}

		ManifoldPoints points;

		if (dim == Dimension::d2)
		{
			auto& func = d2.tests[aid][bid];
			if (func) {
				points = func(a, at, b, bt);
			}
		}

		else 
		if (dim == Dimension::d3)
		{
			auto& func = d3.tests[aid][bid];
			if (func) {
				points = func(a, at, b, bt);
			}
		}

		if (swap && points.HasCollision)
		{
			points.SwapPoints();
		}

		return points;
	}
}

	using namespace Physics;
}
