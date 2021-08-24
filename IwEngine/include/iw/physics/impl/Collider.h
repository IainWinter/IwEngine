#pragma once

#include "iw/physics/AABB.h"
#include "iw/log/logger.h"

namespace iw  {
namespace Physics {
	enum class ColliderType {
		PLANE,
		SPHERE,
		CAPSULE,
		HULL,
		MESH
	};

namespace impl {
	struct ColliderBase {
		const ColliderType Type;
		const Dimension Dim;

		ColliderBase(
			ColliderType type,
			Dimension dim
		)
			: Type(type)
			, Dim (dim)
		{}

		template<typename _t>
		_t* as() {
			assert(_t().get_id() == get_id());
			return (_t*)this;
		}

		size_t get_id() const {
			return (size_t)Type;
		}
	};

	template<
		Dimension _d>
	struct Collider
		: ColliderBase
	{
		using vec_t  = _vec<_d>;
		using aabb_t = AABB<_d>;

	protected:
		aabb_t m_bounds;
		bool   m_boundsOutdated;

	public:
		Collider(
			ColliderType type
		)
			: ColliderBase(type, _d)
			, m_boundsOutdated(true)
		{
			static_assert(_d == d2 || _d == d3);
		}

		const aabb_t& Bounds() const {
			if (m_boundsOutdated) LOG_WARNING << "const Collider::Bounds returned outdated bounds!";
			return m_bounds;
		}

		const aabb_t& Bounds() {
			if (m_boundsOutdated) {
				m_boundsOutdated = false;
				m_bounds = GenerateBounds();
			}

			return m_bounds;
		}

		virtual vec_t FindFurthestPoint(
			const Transform* transform,
			const vec_t&     direction) const = 0; // For GJK
	protected:
		virtual aabb_t GenerateBounds() const = 0;
	};
}
}

	using namespace Physics;
}
