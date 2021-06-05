#pragma once

#include "iw/physics/IwPhysics.h" 
#include "iw/physics/impl/Collider.h"
#include "iw/physics/Collision/ManifoldPoints.h"
#include <array>
#include <vector>
#include <initializer_list>
#include <utility>

namespace iw {
namespace Physics {
namespace impl {

	using namespace glm;

	template<
		Dimension _d>
	struct Simplex {
	public:
		using vec_t = _vec<_d>;
	private:
		std::array<vec_t, _d + 1u> m_points;
		unsigned m_size;

	public:
		Simplex()
			: m_points()
			, m_size(0)
		{
			static_assert(_d == 2 || _d == 3);
		}

		Simplex& operator=(
			std::initializer_list<vec_t> list)
		{
			for (auto v = list.begin(); v != list.end(); v++) {
				m_points[std::distance(list.begin(), v)] = *v;
			}
			m_size = list.size();

			return *this;
		}

		void push_front(
			vec_t point)
		{
			if constexpr (_d == 3) {
				m_points = { point, m_points[0], m_points[1], m_points[2] };
			}

			else
			if constexpr (_d == 2) {
				m_points = { point, m_points[0], m_points[1] };
			}

			m_size = min(m_size + 1, _d + 1u);
		}

		vec_t& operator[](unsigned i) { return m_points[i]; }
		unsigned size() const { return m_size; }

		auto begin() const { return m_points.begin(); }
		auto end()   const { return m_points.end() - (_d + 1u - m_size); }
	};

#ifndef GJK_EPA_MAX_ITER
#	define GJK_EPA_MAX_ITER 32
#endif

	template<
		Dimension _d>
	std::pair<bool, Simplex<_d>> GJK(
		const Collider<_d>* colliderA, const Transform* transformA,
		const Collider<_d>* colliderB, const Transform* transformB)
	{
		using vec_t = _vec<_d>;

		vec_t support = Support<_d>(
			colliderA, transformA,
			colliderB, transformB, vec_t(1));

		Simplex<_d> points;
		points.push_front(support);

		vec_t direction = -support;

		size_t iterations = 0;
		while (iterations++ < GJK_EPA_MAX_ITER) {
			support = Support<_d>(
				colliderA, transformA,
				colliderB, transformB, direction);

			if (dot(support, direction) <= 0) {
				break;
			}

			points.push_front(support);

			if (NextSimplex<_d>(points, direction)) {
				return std::make_pair(true, points);
			}
		}

		return std::make_pair(false, points);
	}

	template<Dimension _d> bool NextSimplex(Simplex<_d>& points, _vec<_d>& direction);
	template<Dimension _d> bool Line       (Simplex<_d>& points, _vec<_d>& direction);
	template<Dimension _d> bool Triangle   (Simplex<_d>& points, _vec<_d>& direction);
	template<Dimension _d> bool Tetrahedron(Simplex<_d>& points, _vec<_d>& direction);

	template<> bool NextSimplex(Simplex<d2>& points, _vec<d2>& direction);
	template<> bool Line       (Simplex<d2>& points, _vec<d2>& direction);
	template<> bool Triangle   (Simplex<d2>& points, _vec<d2>& direction);

	template<> bool NextSimplex(Simplex<d3>& points, _vec<d3>& direction);
	template<> bool Line       (Simplex<d3>& points, _vec<d3>& direction);
	template<> bool Triangle   (Simplex<d3>& points, _vec<d3>& direction);
	template<> bool Tetrahedron(Simplex<d3>& points, _vec<d3>& direction);

	template<
		Dimension _d>
	ManifoldPoints EPA(
		const Simplex <_d>& simplex,
		const Collider<_d>* colliderA, const Transform* transformA,
		const Collider<_d>* colliderB, const Transform* transformB);

	template<>
	ManifoldPoints EPA(
		const Simplex <d2>& simplex,
		const Collider<d2>* colliderA, const Transform* transformA,
		const Collider<d2>* colliderB, const Transform* transformB);

	template<>
	ManifoldPoints EPA(
		const Simplex <d3>& simplex,
		const Collider<d3>* colliderA, const Transform* transformA,
		const Collider<d3>* colliderB, const Transform* transformB);

	template<
		Dimension _d>
	_vec<_d> Support(
		const Collider<_d>* colliderA, const Transform* transformA,
		const Collider<_d>* colliderB, const Transform* transformB,
		const _vec    <_d>& direction)
	{
		return colliderA->FindFurthestPoint(transformA,  direction)
			- colliderB->FindFurthestPoint(transformB, -direction);
	}

	template<
		Dimension _d>
	bool SameDirection(
		const _vec<_d>& direction,
		const _vec<_d>& ao)
	{
		return dot(direction, ao) > 0;
	}

	// For d3 EPA

	std::pair<std::vector<glm::vec4>, size_t> GetFaceNormals(
		const std::vector<glm::vec3>& polytope,
		const std::vector<size_t>&  faces);

	void AddIfUniqueEdge(
		std::vector<std::pair<size_t, size_t>>& edges,
		const std::vector<size_t>& faces,
		size_t a,
		size_t b);
}
}

	using namespace Physics;
}
