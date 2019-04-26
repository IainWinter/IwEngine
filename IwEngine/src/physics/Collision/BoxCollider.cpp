#include "iw/physics/Collision/BoxCollider.h"

namespace IwPhysics {
	BoxCollider::BoxCollider(
		const AABB& bounds)
		: Collider(bounds)
	{}

	iwm::vector3 BoxCollider::FurthestPoint(
		const iwm::matrix4& transformation,
		const iwm::vector3& direction) const
	{
		iwm::vector3 verts[8]{
				Bounds.Min,
				iwm::vector3(Bounds.Min.x, Bounds.Max.y, Bounds.Min.z),
				iwm::vector3(Bounds.Min.x, Bounds.Max.y, Bounds.Max.z),
				iwm::vector3(Bounds.Min.x, Bounds.Min.y, Bounds.Max.z),
				iwm::vector3(Bounds.Max.x, Bounds.Min.y, Bounds.Max.z),
				Bounds.Max,
				iwm::vector3(Bounds.Max.x, Bounds.Max.y, Bounds.Min.z),
				iwm::vector3(Bounds.Max.x, Bounds.Min.y, Bounds.Min.z)
		};

		unsigned char maxVert = 0;
		float maxDist = std::numeric_limits<float>::lowest();

		for (unsigned char i = 0; i < 8; i++) {
			iwm::vector3 vert = verts[i] * transformation;
			float dist = vert.dot(direction);
			if (dist > maxDist) {
				maxDist = dist;
				maxVert = i;
			}
		}

		return verts[maxVert] * transformation;
	}
}
