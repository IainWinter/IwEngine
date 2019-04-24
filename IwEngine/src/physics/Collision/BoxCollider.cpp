#include "iw/physics/Collision/BoxCollider.h"

namespace IwPhysics {
	BoxCollider::BoxCollider(
		const AABB& bounds)
		: Collider(bounds)
	{}

	iwm::vector3 BoxCollider::Support(
		const iwm::matrix4& transformation) const
	{
		iwm::vector3 max;
		
		float distance    = Bounds.Min.dot(Bounds.Min);
		float maxDistance = std::numeric_limits<float>::lowest();
		if (distance > maxDistance) {
			maxDistance = distance;
			max         = Bounds.Min;
		}

		distance = Bounds.Max.dot(Bounds.Max);
		if (distance > maxDistance) {
			maxDistance = distance;
			max         = Bounds.Min;
		}

		iwm::vector3 vector = iwmath::vector3(
			Bounds.Min.x, Bounds.Max.y, Bounds.Min.z);

		distance = vector.dot(vector);
		if (distance > maxDistance) {
			maxDistance = distance;
			max         = vector;
		}

		vector   = iwmath::vector3(Bounds.Min.x, Bounds.Max.y, Bounds.Max.z);
		distance = vector.dot(vector);
		if (distance > maxDistance) {
			maxDistance = distance;
			max         = vector;
		}

		vector   = iwmath::vector3(Bounds.Min.x, Bounds.Min.y, Bounds.Max.z);
		distance = vector.dot(vector);
		if (distance > maxDistance) {
			maxDistance = distance;
			max         = vector;
		}

		vector   = iwmath::vector3(Bounds.Min.x, Bounds.Max.y, Bounds.Max.z);
		distance = vector.dot(vector);
		if (distance > maxDistance) {
			maxDistance = distance;
			max         = vector;
		}

		vector   = iwmath::vector3(Bounds.Min.x, Bounds.Min.y, Bounds.Max.z);
		distance = vector.dot(vector);
		if (distance > maxDistance) {
			maxDistance = distance;
			max         = vector;
		}

		vector   = iwmath::vector3(Bounds.Max.x, Bounds.Max.y, Bounds.Min.z);
		distance = vector.dot(vector);
		if (distance > maxDistance) {
			maxDistance = distance;
			max         = vector;
		}

		vector   = iwmath::vector3(Bounds.Max.x, Bounds.Min.y, Bounds.Min.z);
		distance = vector.dot(vector);
		if (distance > maxDistance) {
			maxDistance = distance;
			max         = vector;
		}

		return max;
	}
}
