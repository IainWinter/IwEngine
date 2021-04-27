#pragma once

#include "Collider.h"
#include <vector>
#include <type_traits>

namespace iw {
namespace Physics {
namespace impl {
	template<
		typename V>
	struct MeshCollider
		: Collider<V>
	{
	private:
		std::vector<V> m_points;

	public:
		MeshCollider(std::vector<V> points = {})
			: Collider<V>(ColliderType::MESH)
			, m_points(points)
		{}

		static MeshCollider MakeCube() {
			MeshCollider collider;
			collider.m_points.push_back(V(-1, -1, -1)); // 0 
			collider.m_points.push_back(V(-1,  1, -1)); // 1 
			collider.m_points.push_back(V( 1,  1, -1)); // 2 
			collider.m_points.push_back(V( 1, -1, -1)); // 3 

			collider.m_points.push_back(V(-1, -1,  1)); // 7
			collider.m_points.push_back(V( 1, -1,  1)); // 6
			collider.m_points.push_back(V( 1,  1,  1)); // 5
			collider.m_points.push_back(V(-1,  1,  1)); // 4

			return collider;
		}

		static MeshCollider MakeTetrahedron() {
			MeshCollider collider;
			collider.m_points.push_back(V(cos(Pi2 * 0 / 3), -1, sin(Pi2 * 0 / 3)));
			collider.m_points.push_back(V(cos(Pi2 * 1 / 3), -1, sin(Pi2 * 1 / 3)));
			collider.m_points.push_back(V(cos(Pi2 * 2 / 3), -1, sin(Pi2 * 2 / 3)));
			collider.m_points.push_back(V(0, 1, 0));

			return collider;
		}

		const AABB<V>& Bounds() { return AABB<V>(); }
		Transform Trans() const { return Transform(); }

		V FindFurthestPoint(
			const Transform* transform,
			V direction) const override
		{
			//glm::vec4 dir = glm::vec4(direction, 1);
			//dir *= transform->WorldTransformation().inverted(); // I think we can transform the direction instead of the collider here

			//direction = dir.xyz();

			V     maxPoint;
			float maxDistance = -FLT_MAX;

			for (V point : m_points) {
				point = V(glm::vec4(point, 1) * glm::transpose(transform->WorldTransformation()));

				float distance = glm::dot(point, direction);
				if (distance > maxDistance) {
					maxDistance = distance;
					maxPoint = point;
				}
			}

			//glm::vec4 mp = glm::vec4(maxPoint, 1);  // and then transform the final point?
			//mp *= transform->WorldTransformation();

			return maxPoint;//mp.xyz();
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
//struct MeshCollider
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
