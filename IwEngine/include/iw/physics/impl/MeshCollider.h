#pragma once

#include "HullCollider.h"

namespace iw {
namespace Physics {
namespace impl {
	template<
		Dimension _d>
	struct MeshCollider
		: HullCollider<_d>
	{
		using vec_t  = _vec<_d>;
		using aabb_t = AABB<_d>;
		using hull_t = HullCollider<_d>;

		std::vector<unsigned> Triangles;
		std::vector<hull_t> ConvexParts;

		MeshCollider(
			const std::vector<vec_t>& points = {},
			const std::vector<unsigned>& triangles = {}
		)
			: hull_t(points, true)
			, Triangles(triangles)
		{}

		void AddTriangle(
			unsigned a,
			unsigned b,
			unsigned c)
		{
			Triangles.push_back(a);
			Triangles.push_back(b);
			Triangles.push_back(c);
		}

		void RemoveTriangle(
			unsigned a,
			unsigned b,
			unsigned c)
		{
			for (auto itr = Triangles.begin(); itr != Triangles.end(); itr += 3)
			{
				if (    *(itr)     == a
					&& *(itr + 1) == b
					&& *(itr + 2) == c)
				{
					Triangles.erase(itr); // ok because we break right after
					break;
				}
			}
		}

		bool CacheIsOld() const override
		{
			size_t size = Triangles.size();

			// could check point size too, double check but could exit earlier,
			// not sure if the optimizer is smart enough to do this???

			if (size != t_triangles.size())
			{
				return true;
			}

			for (size_t i = 0; i < size; i++)
			{
				if (Triangles.at(i) != t_triangles.at(i))
				{
					return true;
				}
			}

			return false;
		}

		void UpdateCache() override
		{
			t_triangles = Triangles;

			ConvexParts.clear();

			for (size_t i = 0; i < Triangles.size(); i += 3)
			{
				ConvexParts.push_back(hull_t({
					Points[Triangles[i]],
					Points[Triangles[i + 1]],
					Points[Triangles[i + 2]],
				}));
			}

			Collider<_d>::UpdateCache();
		}
	private:
		std::vector<unsigned> t_triangles; // expensive but allows public Points
	};
}
}

	using namespace Physics;
}

//#include <array>
//
//
//namespace vvv {
//	using namespace iw;
//
//struct Collider {
//	virtual glm::vec3 FindFurthestPoint(
//		glm::vec3 direction) const = 0;
//};
//
//struct HullCollider
//	: Collider
//{
//private:
//	std::vector<glm::vec3> Points;
//
//public:
//	glm::vec3 FindFurthestPoint(
//		glm::vec3 direction) const override
//	{
//		glm::vec3 maxPoint;
//		float   maxDistance = -FLT_MAX;
//
//		for (glm::vec3 point : Points) {
//			float distance = point.dot(direction);
//			if (distance > maxDistance) {
//				maxDistance = distance;
//				maxPoint = point;
//			}
//		}
//
//		return maxPoint;
//	}
//};
//
//glm::vec3 Support(
//	const Collider* colliderA,
//	const Collider* colliderB,
//	glm::vec3 direction)
//{
//	return colliderA->FindFurthestPoint( direction)
//		- colliderB->FindFurthestPoint(-direction);
//}
//
//struct Simplex {
//private:
//	std::array<glm::vec3, 4> Points;
//	unsigned m_size;
//
//public:
//	Simplex()
//		: Points({ 0, 0, 0, 0 })
//		, m_size(0)
//	{}
//
//	Simplex& operator=(std::initializer_list<glm::vec3> list) {
//		for (auto* v = list.begin(); v != list.end(); v++) {
//			Points[std::distance(list.begin(), v)] = *v;
//		}
//		m_size = list.size();
//
//		return *this;
//	}
//
//	void push_front(glm::vec3 point) {
//		Points = { point, Points[0], Points[1], Points[2] };
//		m_size = m_size == 4 ? 4 : m_size + 1;
//	}
//
//	glm::vec3& operator[](unsigned i) { return Points[i]; }
//	unsigned size() const { return m_size; }
//
//	auto begin() const { return Points.begin(); }
//	auto end()   const { return Points.end() - (4 - m_size); }
//};
//
//bool Line(
//	Simplex& points,
//	glm::vec3& direction) { return false; }
//
//bool Triangle(
//	Simplex& points,
//	glm::vec3& direction) { return false; }
//
//bool Tetrahedron(
//	Simplex& points,
//	glm::vec3& direction) { return false; }
//
//bool NextSimplex(
//	Simplex& points,
//	glm::vec3& direction)
//{
//	switch (points.size()) {
//		case 2: return Line       (points, direction);
//		case 3: return Triangle   (points, direction);
//		case 4: return Tetrahedron(points, direction);
//	}
//
//	// never should be here
//	return false;
//}
//
//
//bool GJK(
//	const Collider* colliderA,
//	const Collider* colliderB)
//{
//	// Get initial support point in any direction
//	glm::vec3 support = Support(colliderA, colliderB, glm::vec3::unit_x);
//
//	// Simplex is an array of points, max count is 4
//	Simplex points;
//	points.push_front(support);
//
//	// New direction is towards the origin
//	glm::vec3 direction = -support;
//
//	while (true) {
//		support = Support(colliderA, colliderB, direction);
//
//		if (support.dot(direction) <= 0) {
//			return false; // no collision
//		}
//
//		points.push_front(support);
//
//		if (NextSimplex(points, direction)) {
//			return true;
//		}
//	}
//}
//
//}
