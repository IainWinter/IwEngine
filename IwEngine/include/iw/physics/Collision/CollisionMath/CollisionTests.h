#pragma once

#include "iw/physics/IwPhysics.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/Manifold.h"
#include "iw/physics/Collision/CollisionObject.h"
#include <tuple>

namespace IW {
namespace Physics {
namespace algo {


	//template<
	//	typename V>
	//bool TestCollision(
	//	const SphereCollider &left,
	//	const SphereCollider &right,
	//	V *resolve = nullptr)
	//{
	//	double r = left.Radius + right.Radius;
	//	V d = left.Center - right.Center;
	//	if (d.length() < r) {
	//		if(resolve != nullptr) *resolve = (d / d.length()) * (r - d.length());
	//		return true;
	//	}
	//	return false;
	//}

	//bool TestCollision(
	//	const SphereCollider& left,
	//	const SphereCollider2& right,
	//	iw::vector3* resolve = nullptr)
	//{
	//	return false;
	//}

	//bool TestCollision(
	//	const BoxCollider<iw::vector2>& left,
	//	const BoxCollider<iw::vector2>& right,
	//	iw::vector2 *resolve = nullptr)
	//{
	//	return false;
	//}

	using ManifoldPointz = std::tuple<iw::vector3, iw::vector3>;

	Manifold MakeManiFold(
		const Rigidbody* a,
		const Rigidbody* b)
	{
		ManifoldPointz pz = FindManifoldPoints(a->Col, b->Col);
		return {
			std::get<0>(pz),
			std::get<1>(pz),
			a,
			b,
			(std::get<0>(pz) - std::get<1>(pz)).length()
		};

	}

	ManifoldPointz FindManifoldPoints(
		const SphereCollider *a,
		const SphereCollider *b)
	{

		iw::vector3 BtoA = a->Center - b->Center;
		BtoA.normalize();
		
		iw::vector3 AtoB = b->Center - a->Center;
		AtoB.normalize();

		return {
			AtoB * a->Radius,
			BtoA * b->Radius
		};
	}

}
}

	using namespace Physics;
}
