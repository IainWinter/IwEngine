#pragma once

#include "iw/physics/IwPhysics.h" 
#include "iw/physics/Collision/Collider.h"
#include <array>

namespace iw {
namespace Physics {
namespace algo {
	struct Simplex {
	private:
		std::array<vector3, 4> m_points;
		unsigned m_size;

	public:
		Simplex()
			: m_points({ 0, 0, 0, 0 })
			, m_size(0)
		{}

		Simplex& operator=(std::initializer_list<vector3> list) {
			for (auto v = list.begin(); v != list.end(); v++) {
				m_points[std::distance(list.begin(), v)] = *v;
			}
			m_size = list.size();

			return *this;
		}

		void push_front(vector3 point) {
			m_points = { point, m_points[0], m_points[1], m_points[2] };
			m_size = std::min(m_size + 1, 4u);
		}

		vector3& operator[](unsigned i) { return m_points[i]; }
		unsigned size() const { return m_size; }

		auto begin() const { return m_points.begin(); }
		auto end()   const { return m_points.end() - (4 - m_size); }
	};

	std::pair<bool, Simplex> GJK(
		const Collider* colliderA, const Transform* transformA,
		const Collider* colliderB, const Transform* transformB);

	ManifoldPoints EPA(
		const Simplex& simplex,
		const Collider* colliderA, const Transform* transformA,
		const Collider* colliderB, const Transform* transformB);

namespace detail {
	iw::vector3 Support(
		const Collider* colliderA, const Transform* transformA,
		const Collider* colliderB, const Transform* transformB,
		vector3 direction);

	bool NextSimplex(
		Simplex& points,
		vector3& direction);

	bool Line(
		Simplex& points,
		vector3& direction);

	bool Triangle(
		Simplex& points,
		vector3& direction);

	bool Tetrahedron(
		Simplex& points,
		vector3& direction);

	bool SameDirection(
		const iw::vector3& direction,
		const iw::vector3& ao);
}
}
}

	using namespace Physics;
}
