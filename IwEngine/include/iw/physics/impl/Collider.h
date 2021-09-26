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
	struct ColliderBase;

	template<Dimension _d>
	struct Collider;

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

		template<Dimension _d>
		Collider<_d>* as_dim() {
			assert(_d == Dim);
			return (Collider<_d>*)this;
		}

		size_t get_id() const {
			return (size_t)Type;
		}

		virtual bool CacheIsOld() const = 0;
		virtual void UpdateCache() = 0;
	};

	template<
		Dimension _d>
	struct Collider
		: ColliderBase
	{
		using vec_t  = _vec<_d>;
		using aabb_t = AABB<_d>;

		Collider(
			ColliderType type
		)
			: ColliderBase(type, _d)
		{
			static_assert(_d == d2 || _d == d3);
		}

		virtual vec_t FindFurthestPoint(
			Transform* transform,
			const vec_t& direction) const = 0;

		virtual aabb_t CalcBounds() const = 0;

		aabb_t Bounds()
		{
			if (CacheIsOld())
			{
				UpdateCache();
			}

			return t_bounds;
		}

		void UpdateCache() override
		{
			t_bounds = CalcBounds();
		}
	private:
		aabb_t t_bounds;
	};
}
}

	using namespace Physics;
}
