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

		std::vector<unsigned> m_index;
	private:
		std::vector<hull_t> m_parts;
		bool m_partsOutdated;

	public:
		MeshCollider(
			const std::vector<vec_t>&    points = {},
			const std::vector<unsigned>& index  = {}
		)
			: hull_t(points, true)
			, m_index(index)
			, m_partsOutdated(true)
		{}

		void SetTriangles(
			const std::vector<unsigned>& index)
		{
			m_index = index;
			m_partsOutdated = true;
		}

		void AddTriangle(
			unsigned a,
			unsigned b,
			unsigned c)
		{
			m_index.push_back(a);
			m_index.push_back(b);
			m_index.push_back(c);
			m_partsOutdated = true;
		}

		void RemoveTriangle(
			unsigned a,
			unsigned b,
			unsigned c)
		{
			for (auto itr = m_index.begin(); itr != m_index.end(); itr += 3)
			{
				if (    *(itr)     == a
					&& *(itr + 1) == b
					&& *(itr + 2) == c)
				{
					m_index.erase(itr); // ok because we break right after
					break;
				}
			}

			m_partsOutdated = true;
		}

		const std::vector<hull_t>& GetHullParts() const { // For now the parts are the triangles, could merge into bigger parts
			if (!m_partsOutdated) LOG_WARNING << "Called const MeshCollider::GetHullParts with outdated parts";
			return m_parts;
		}

		std::vector<hull_t>& GetHullParts() {
			if (!m_partsOutdated) return m_parts;

			m_partsOutdated = false;
			m_parts.clear();

			for (size_t i = 0; i < m_index.size(); i += 3)
			{
				m_parts.push_back(hull_t({
					m_points[m_index[i    ]],
					m_points[m_index[i + 1]],
					m_points[m_index[i + 2]],
				}));
			}

			return m_parts;
		}
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
//	std::vector<glm::vec3> m_points;
//
//public:
//	glm::vec3 FindFurthestPoint(
//		glm::vec3 direction) const override
//	{
//		glm::vec3 maxPoint;
//		float   maxDistance = -FLT_MAX;
//
//		for (glm::vec3 point : m_points) {
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
//	std::array<glm::vec3, 4> m_points;
//	unsigned m_size;
//
//public:
//	Simplex()
//		: m_points({ 0, 0, 0, 0 })
//		, m_size(0)
//	{}
//
//	Simplex& operator=(std::initializer_list<glm::vec3> list) {
//		for (auto* v = list.begin(); v != list.end(); v++) {
//			m_points[std::distance(list.begin(), v)] = *v;
//		}
//		m_size = list.size();
//
//		return *this;
//	}
//
//	void push_front(glm::vec3 point) {
//		m_points = { point, m_points[0], m_points[1], m_points[2] };
//		m_size = m_size == 4 ? 4 : m_size + 1;
//	}
//
//	glm::vec3& operator[](unsigned i) { return m_points[i]; }
//	unsigned size() const { return m_size; }
//
//	auto begin() const { return m_points.begin(); }
//	auto end()   const { return m_points.end() - (4 - m_size); }
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
