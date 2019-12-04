#pragma once

namespace IW  {
inline namespace Physics {
	struct BoxCollider;
	struct SphereCollider;
	struct SquareCollider;
	struct CircleCollider;
	struct MeshCollider;
	template<
		typename V>
	struct Collider {
		size_t Count;
		iw::vector3* Points;

		Collider(
			size_t count,
			V* points)
		{
			Count = count;
			Points = points;
		}

		virtual bool IsColliding(
			const IW::BoxCollider &other) const = 0;

		virtual bool IsColliding(
			const IW::SphereCollider &other) const = 0;

		virtual bool IsColliding(
			const IW::SquareCollider &other) const = 0;

		virtual bool IsColliding(
			const IW::CircleCollider &other) const = 0;

		virtual bool IsColliding(
			const IW::MeshCollider &other) const = 0;

		virtual IW:AABB GenAABB() = 0;
	};
}
}
