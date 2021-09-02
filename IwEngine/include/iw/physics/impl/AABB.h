#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/common/Components/Transform.h"
#include "iw/common/algos/polygon2.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		Dimension _d>
	struct REFLECT AABB {
		using vec_t = _vec<_d>;
		using pair_t = std::pair<vec_t, vec_t>;

		REFLECT vec_t Min;
		REFLECT vec_t Max;

		AABB() 
			: Min( FLT_MAX)
			, Max(-FLT_MAX)
		{}

		AABB(
			const vec_t& center,
			float scale
		)
			: Min(center - scale)
			, Max(center + scale) // was / 2 before might be better
		{}

		AABB(
			const vec_t& p1,
			const vec_t& p2)
		{
			*this = FromPair(pair_t(p1, p2));
		}

		AABB(
			const pair_t& ps)
		{
			*this = FromPair(ps);
		}

		AABB& operator=(
			const pair_t& ps)
		{
			*this = FromPair(ps);
			return *this;
		}

		operator pair_t() const
		{
			return pair_t(Min, Max);
		}

		vec_t Center() const {
			return (Min + Max) / scalar(2);
		}

		bool Intersects(
			                   Transform* transform,
			const AABB& other, Transform* otherTransform) const
		{
			AABB me   = TransformBounds(*this, transform);
			AABB them = TransformBounds(other, otherTransform);

			for (int i = 0; i < int(_d); i++) {
				if (me.Min[i] > them.Max[i] || me.Max[i] < them.Min[i]) return false;
			}

			return true;
		}

		bool Fits(
			                   Transform* transform,
			const AABB& other, Transform* otherTransform) const
		{
			AABB me   = TransformBounds(*this, transform);
			AABB them = TransformBounds(other, otherTransform);

			for (int i = 0; i < int(_d); i++) {
				if (me.Min[i] >= them.Min[i] || me.Max[i] <= them.Max[i]) return false;
			}

			return true;
		}

	private:
		static AABB FromPair(
			const std::pair<_vec<d2>, _vec<d2>>& ps)
		{
			const auto& [p1, p2] = ps;

			bool p1x = p1.x < p2.x;
			bool p1y = p1.y < p2.y;

			AABB aabb;
			aabb.Min.x =  p1x ? p1.x : p2.x;
			aabb.Min.y =  p1y ? p1.y : p2.y;
			aabb.Max.x = !p1x ? p1.x : p2.x;
			aabb.Max.y = !p1y ? p1.y : p2.y;

			return aabb;
		}

		static AABB FromPair(
			const std::pair<_vec<d3>, _vec<d3>>& ps)
		{
			const auto& [p1, p2] = ps;

			bool p1x = p1.x < p2.x;
			bool p1y = p1.y < p2.y;
			bool p1z = p1.z < p2.z;

			AABB aabb;
			aabb.Min.x =  p1x ? p1.x : p2.x;
			aabb.Min.y =  p1y ? p1.y : p2.y;
			aabb.Min.z =  p1z ? p1.z : p2.z;
			aabb.Max.x = !p1x ? p1.x : p2.x;
			aabb.Max.y = !p1y ? p1.y : p2.y;
			aabb.Max.z = !p1z ? p1.z : p2.z;

			return aabb;
		}
	};
}
}

	using namespace Physics;
}
