#pragma once

#include "Collider.h"
#include "iw/common/algos/polygon2.h"
#include <vector>

namespace iw {
namespace Physics {
namespace impl {
	template<
		Dimension _d>
	struct HullCollider
		: Collider<_d>
	{
		using vec_t = _vec<_d>;
		using aabb_t = AABB<_d>;

		std::vector<vec_t> Points;

		HullCollider(
			const std::vector<vec_t>& points = {},
			bool isMesh = false // this is bad
		)
			: Collider<_d>(isMesh ? ColliderType::MESH : ColliderType::HULL)
			, Points(points)
		{
			Bounds();
		}

		virtual void AddPoint(
			const vec_t& p)
		{
			Points.push_back(p);
		}

		virtual void RemovePoint(
			const vec_t& p)
		{
			Points.erase(std::find(Points.begin(), Points.end(), p));
		}

		vec_t FindFurthestPoint(
			Transform* transform,
			const vec_t&     direction) const override
		{
			//glm::vec4 dir = glm::vec4(direction, 1);
			//dir *= transform->WorldTransformation().inverted(); // I think we can transform the direction instead of the collider here

			//direction = dir.xyz();

			vec_t maxPoint = vec_t(0);
			float maxDistance = -FLT_MAX;

			for (const vec_t& point : Points)
			{
				vec_t p = TransformPoint<_d>(point, transform);

				float distance = glm::dot(p, direction);
				if (distance > maxDistance) {
					maxDistance = distance;
					maxPoint    = p;
				}
			}

			//glm::vec4 mp = glm::vec4(maxPoint, 1);  // and then transform the final point?
			//mp *= transform->WorldTransformation();

			return maxPoint;//mp.xyz();


			//if (Points.size() == 0) return vec_t(0);

			//_vec<d3> d;
			//if constexpr (_d == d2) d = _vec<d3>(direction, 0);
			//else                    d = direction;

			//vec_t dir = (vec_t)(d * transform->WorldRotation());

			//vec_t maxPoint;
			//float maxDistance = FLT_MIN;

			//for (vec_t point : Points) {
			//	float distance = glm::dot(point, dir);
			//	if (distance > maxDistance) {
			//		maxDistance = distance;
			//		maxPoint    = point;
			//	}
			//}

			//glm::vec4 v;

			//if constexpr (_d == d2) v = glm::vec4(maxPoint, 0, 1);
			//else                    v = glm::vec4(maxPoint,    1);

			//return vec_t(v * transform->WorldTransformation());
		}

		aabb_t CalcBounds() const
		{
			return GenPolygonBounds(Points);
		}

		bool CacheIsOld() const override
		{
			size_t size = Points.size();

			if (size != t_points.size())
			{
				return true;
			}

			// only actually need to test points on bounds...
			// but that would be annoying lol, could save index of
			// points from GenPolygonBounds.
			// note though: this gives MeshCollider a way to Recalc its peices
			// bc that needs to test every point

			for (size_t i = 0; i < size; i++)
			{
				if (Points.at(i) != t_points.at(i))
				{
					return true;
				}
			}

			return false;
		}

		void UpdateCache()
		{
			t_points = Points;
		}
	private:
		std::vector<vec_t> t_points; // expensive but allows public Points
	};
}

	inline impl::HullCollider<d3> MakeCubeCollider() {
		impl::HullCollider<d3> collider;
		collider.AddPoint(glm::vec3(-1, -1, -1)); // 0 
		collider.AddPoint(glm::vec3(-1,  1, -1)); // 1 
		collider.AddPoint(glm::vec3( 1,  1, -1)); // 2 
		collider.AddPoint(glm::vec3( 1, -1, -1)); // 3 

		collider.AddPoint(glm::vec3(-1, -1,  1)); // 7
		collider.AddPoint(glm::vec3( 1, -1,  1)); // 6
		collider.AddPoint(glm::vec3( 1,  1,  1)); // 5
		collider.AddPoint(glm::vec3(-1,  1,  1)); // 4

		return collider;
	}

	inline impl::HullCollider<d3> MakeTetrahedronCollider() {
		impl::HullCollider<d3> collider;
		collider.AddPoint(glm::vec3(cos(Pi2 * 0 / 3), -1, sin(Pi2 * 0 / 3)));
		collider.AddPoint(glm::vec3(cos(Pi2 * 1 / 3), -1, sin(Pi2 * 1 / 3)));
		collider.AddPoint(glm::vec3(cos(Pi2 * 2 / 3), -1, sin(Pi2 * 2 / 3)));
		collider.AddPoint(glm::vec3(0, 1, 0));

		return collider;
	}

	inline impl::HullCollider<d2> MakeSquareCollider() {
		impl::HullCollider<d2> collider;
		collider.AddPoint(glm::vec2(-1,  1)); // 0 
		collider.AddPoint(glm::vec2(-1, -1)); // 1 
		collider.AddPoint(glm::vec2( 1, -1)); // 2 
		collider.AddPoint(glm::vec2( 1,  1)); // 3 

		return collider;
	}

	inline impl::HullCollider<d2> MakeTriangleCollider() {
		impl::HullCollider<d2> collider;
		collider.AddPoint(glm::vec2(cos(Pi2 * 0 / 3), sin(Pi2 * 0 / 3)));
		collider.AddPoint(glm::vec2(cos(Pi2 * 1 / 3), sin(Pi2 * 1 / 3)));
		collider.AddPoint(glm::vec2(cos(Pi2 * 2 / 3), sin(Pi2 * 2 / 3)));

		return collider;
	}
}

	using namespace Physics;
}
