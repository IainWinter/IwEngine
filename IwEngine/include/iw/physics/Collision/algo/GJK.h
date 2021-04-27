#pragma once

#include "iw/physics/IwPhysics.h" 
#include "iw/physics/Collision/Collider.h"
#include <array>
#include <vector>

namespace iw {
namespace Physics {
namespace algo {
	struct Simplex {
	private:
		std::array<glm::vec3, 4> m_points;
		unsigned m_size;

	public:
		Simplex()
			: m_points(/*{ 0, 0, 0, 0 }*/)
			, m_size(0)
		{}

		Simplex& operator=(std::initializer_list<glm::vec3> list) {
			for (auto v = list.begin(); v != list.end(); v++) {
				m_points[std::distance(list.begin(), v)] = *v;
			}
			m_size = list.size();

			return *this;
		}

		void push_front(glm::vec3 point) {
			m_points = { point, m_points[0], m_points[1], m_points[2] };
			m_size = glm::min(m_size + 1, 4u);
		}

		glm::vec3& operator[](unsigned i) { return m_points[i]; }
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
	glm::vec3 Support(
		const Collider* colliderA, const Transform* transformA,
		const Collider* colliderB, const Transform* transformB,
		glm::vec3 direction);

	std::pair<std::vector<glm::vec4>, size_t> GetFaceNormals(
		const std::vector<glm::vec3>& polytope,
		const std::vector<size_t>&  faces);

	void AddIfUniqueEdge(
		std::vector<std::pair<size_t, size_t>>& edges,
		const std::vector<size_t>& faces,
		size_t a,
		size_t b);

	bool NextSimplex(
		Simplex& points,
		glm::vec3& direction);

	bool Line(
		Simplex& points,
		glm::vec3& direction);

	bool Triangle(
		Simplex& points,
		glm::vec3& direction);

	bool Tetrahedron(
		Simplex& points,
		glm::vec3& direction);

	bool SameDirection(
		const glm::vec3& direction,
		const glm::vec3& ao);
}
}
}

	using namespace Physics;
}
