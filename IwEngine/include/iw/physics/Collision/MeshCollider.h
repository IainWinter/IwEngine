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
		MeshCollider()
			: Collider<V>(ColliderType::MESH)
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

		//IWPHYSICS_API
		const AABB<V>& Bounds() { return AABB<V>(); }

		//IWPHYSICS_API
		Transform Trans() const { return Transform(); }

		IWPHYSICS_API
		V FindFurthestPoint(
			const Transform* transform,
			V direction) const override;

		ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::Collider<V>* collider,
			const Transform* colliderTransform) const override
		{
			return collider->TestCollision(colliderTransform, this, transform);
		}

		IWPHYSICS_API
		ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::SphereCollider<V>* sphere,
			const Transform* sphereTransform) const override;

		IWPHYSICS_API
		ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::CapsuleCollider<V>* capsule,
			const Transform* capsuleTransform) const override;

		IWPHYSICS_API
		ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::PlaneCollider<V>* plane,
			const Transform* planeTransform) const override;

		IWPHYSICS_API
		ManifoldPoints TestCollision(
			const Transform* transform,
			const impl::MeshCollider<V>* mesh,
			const Transform* meshTransform) const override;
	};
}

	using MeshCollider2 = impl::MeshCollider<iw::vector2>;
	using MeshCollider  = impl::MeshCollider<iw::vector3>;
	using MeshCollider4 = impl::MeshCollider<iw::vector4>;
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
//	virtual vector3 FindFurthestPoint(
//		vector3 direction) const = 0;
//};
//
//struct MeshCollider
//	: Collider
//{
//private:
//	std::vector<vector3> m_points;
//
//public:
//	vector3 FindFurthestPoint(
//		vector3 direction) const override
//	{
//		vector3 maxPoint;
//		float   maxDistance = -FLT_MAX;
//
//		for (vector3 point : m_points) {
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
//vector3 Support(
//	const Collider* colliderA,
//	const Collider* colliderB,
//	vector3 direction)
//{
//	return colliderA->FindFurthestPoint( direction)
//		- colliderB->FindFurthestPoint(-direction);
//}
//
//struct Simplex {
//private:
//	std::array<vector3, 4> m_points;
//	unsigned m_size;
//
//public:
//	Simplex()
//		: m_points({ 0, 0, 0, 0 })
//		, m_size(0)
//	{}
//
//	Simplex& operator=(std::initializer_list<vector3> list) {
//		for (auto* v = list.begin(); v != list.end(); v++) {
//			m_points[std::distance(list.begin(), v)] = *v;
//		}
//		m_size = list.size();
//
//		return *this;
//	}
//
//	void push_front(vector3 point) {
//		m_points = { point, m_points[0], m_points[1], m_points[2] };
//		m_size = m_size == 4 ? 4 : m_size + 1;
//	}
//
//	vector3& operator[](unsigned i) { return m_points[i]; }
//	unsigned size() const { return m_size; }
//
//	auto begin() const { return m_points.begin(); }
//	auto end()   const { return m_points.end() - (4 - m_size); }
//};
//
//bool Line(
//	Simplex& points,
//	vector3& direction) { return false; }
//
//bool Triangle(
//	Simplex& points,
//	vector3& direction) { return false; }
//
//bool Tetrahedron(
//	Simplex& points,
//	vector3& direction) { return false; }
//
//bool NextSimplex(
//	Simplex& points,
//	vector3& direction)
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
//	vector3 support = Support(colliderA, colliderB, vector3::unit_x);
//
//	// Simplex is an array of points, max count is 4
//	Simplex points;
//	points.push_front(support);
//
//	// New direction is towards the origin
//	vector3 direction = -support;
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
