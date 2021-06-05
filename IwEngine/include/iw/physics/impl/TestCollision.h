#pragma once

#include "..\Collision\ManifoldPoints.h"
#include "PlaneCollider.h"
#include "SphereCollider.h"
#include "CapsuleCollider.h"
#include "HullCollider.h"
#include "MeshCollider.h"
#include "GJK.h"

#include "iw/math/iwmath.h"

namespace iw {
namespace Physics {
namespace impl {

	using namespace glm;

	using Test_Collision_func = ManifoldPoints(*)(
		const ColliderBase*, const Transform*,
		const ColliderBase*, const Transform*);

	template<
		Dimension _d>
	struct Test_Collision_funcs
	{
		impl::Test_Collision_func tests[5][5] = {
			{nullptr, impl::Test_Plane_Sphere <_d>, impl::Test_Plane_Capsule  <_d>, impl::Test_Plane_Hull_Mesh<_d>, impl::Test_Plane_Hull_Mesh<_d> },
			{nullptr, impl::Test_Sphere_Sphere<_d>, impl::Test_Sphere_Capsule <_d>, impl::Test_GJK            <_d>, impl::Test_X_Mesh         <_d> },
			{nullptr, nullptr,                      impl::Test_Capsule_Capsule<_d>, impl::Test_GJK            <_d>, impl::Test_X_Mesh         <_d> },
			{nullptr, nullptr,                      nullptr,                        impl::Test_GJK            <_d>, impl::Test_X_Mesh         <_d> },
			{nullptr, nullptr,                      nullptr,                        nullptr,                        impl::Test_Mesh_Mesh      <_d> },
		};
	};

	template<
		Dimension _d>
	_vec<_d> rot_vec(
		_vec<_d> v,
		const Transform* t)
	{
		if constexpr (_d == d3) {
			v = v * t->WorldRotation();
		}

		else {
			v = (_vec<_d>)(_vec<d3>(v, 0) * t->WorldRotation());
		}

		return v;
	}

	template<
		Dimension _d>
	ManifoldPoints Test_Plane_Sphere(
		const ColliderBase* a, const Transform* at,
		const ColliderBase* b, const Transform* bt)
	{
		assert( a->Type == ColliderType::PLANE
			&& b->Type == ColliderType::SPHERE
			&& a->Dim  == _d
			&& b->Dim  == _d);

		using Plane  = PlaneCollider<_d>;
		using Sphere = SphereCollider<_d>;
		using vec_t  = _vec<_d>;

		Plane*  A = (Plane*)a;
		Sphere* B = (Sphere*)b;

		vec_t  aCenter = B->Center + (vec_t)bt->WorldPosition();
		scalar aRadius = B->Radius * major(bt->WorldScale());

		vec_t normal  = rot_vec<_d>(normalize(A->Normal), at);
		vec_t onPlane = normal * A->Distance + (vec_t)at->WorldPosition();

		scalar distance = dot(aCenter - onPlane, normal); // distance from center of sphere to plane surface

		if (distance > aRadius) {
			return {};
		}
		
		vec_t aDeep = aCenter - normal * aRadius;
		vec_t bDeep = aCenter - normal * distance;

		return ManifoldPoints(aDeep, bDeep, normal, distance);
	}

	template<
		Dimension _d>
	ManifoldPoints Test_Plane_Capsule(
		const ColliderBase* a, const Transform* at,
		const ColliderBase* b, const Transform* bt)
	{
		assert( a->Type == ColliderType::PLANE
			&& b->Type == ColliderType::CAPSULE
			&& a->Dim  == _d
			&& b->Dim  == _d);

		using Plane  = PlaneCollider<_d>;
		using Capsule = CapsuleCollider<_d>;

		Plane*   A = (Plane*)a;
		Capsule* B = (Capsule*)b;

		return {};
	}

	template<
		Dimension _d>
	ManifoldPoints Test_Plane_Hull_Mesh(
		const ColliderBase* a, const Transform* at,
		const ColliderBase* b, const Transform* bt)
	{
		assert(  a->Type == ColliderType::PLANE
			&& (b->Type == ColliderType::HULL || b->Type == ColliderType::MESH)
			&&  a->Dim  == _d
			&&  b->Dim  == _d);

		using Plane = PlaneCollider<_d>;
		using Hull  = HullCollider<_d>;
		using vec_t = _vec<_d>;

		Plane* A = (Plane*)a;
		Hull*  B = (Hull*)b;

		vec_t normal = rot_vec<_d>(normalize(A->Normal), at);

		vec_t plane = normal * A->Distance + (vec_t)at->WorldPosition();
		vec_t bDeep = B->FindFurthestPoint(bt, -normal);

		vec_t ba = plane - bDeep;

		float distance = dot(ba, normal);

		if (distance < 0) {
			return ManifoldPoints();
		}

		// Might nudge 'plane' twoards bDeep (furthest point of plane in B)

		return ManifoldPoints(plane, bDeep, -normal, distance);
	}

	template<
		Dimension _d>
	ManifoldPoints Test_Sphere_Sphere(
		const ColliderBase* a, const Transform* at,
		const ColliderBase* b, const Transform* bt)
	{
		assert( a->Type == ColliderType::SPHERE
			&& b->Type == ColliderType::SPHERE
			&& a->Dim  == _d
			&& b->Dim  == _d);

		using Sphere = SphereCollider<_d>;
		using vec_t  = _vec<_d>;

		Sphere* A = (Sphere*)a;
		Sphere* B = (Sphere*)b;

		vec_t aCenter = A->Center + (vec_t)at->WorldPosition();
		vec_t bCenter = B->Center + (vec_t)bt->WorldPosition();

		vec_t ab = bCenter - aCenter;

		scalar aRadius = A->Radius * major(at->WorldScale());
		scalar bRadius = B->Radius * major(bt->WorldScale());

		scalar distance = length(ab);

		if (distance > aRadius + bRadius) {
			return {};
		}

		vec_t normal = normalize(ab);

		vec_t aDeep = aCenter + normal * aRadius;
		vec_t bDeep = bCenter - normal * bRadius;

		return ManifoldPoints(aDeep, bDeep, normal, distance);
	}

	template<
		Dimension _d>
	ManifoldPoints Test_Sphere_Capsule(
		const ColliderBase* a, const Transform* at,
		const ColliderBase* b, const Transform* bt)
	{
		assert( a->Type == ColliderType::SPHERE
			&& b->Type == ColliderType::CAPSULE
			&& a->Dim  == _d
			&& b->Dim  == _d);

		using Sphere  = SphereCollider<_d>;
		using Capsule = CapsuleCollider<_d>;
		using vec_t   = _vec<_d>;

		Sphere*  A = (Sphere*)a;
		Capsule* B = (Capsule*)b;

		vec_t bScale = bt->WorldScale();

		scalar bHeightScale = bScale.y;
		scalar bRadiusScale = bScale.x;

		if constexpr (_d == d3) {
			bRadiusScale = iw::max(bRadiusScale, bScale.z);
		}

		vec_t y(0); y[1] = 1;

		vec_t bOffset = rot_vec<_d>(y, bt) * (B->Height * bHeightScale / 2 - B->Radius * bRadiusScale);

		vec_t aCenter = A->Center           + (vec_t)at->WorldPosition();
		vec_t bBottom = B->Center - bOffset + (vec_t)bt->WorldPosition();
		vec_t bTop    = B->Center + bOffset + (vec_t)bt->WorldPosition();
		
		scalar aRadius = A->Radius * max(bHeightScale, bRadiusScale);
		scalar bRadius = B->Radius * bRadiusScale;

		vec_t ba   = aCenter - bBottom;
		vec_t bbt  = bTop - bBottom;
		vec_t nbbt = normalize(bbt);

		vec_t bCenterProj = bBottom + nbbt * iw::clamp(dot(nbbt, ba), 0.0f, length(bbt));

		vec_t bp = aCenter - bCenterProj;

		scalar distance = length(bp);

		if (distance > aRadius + bRadius) {
			return {};
		}

		vec_t normal = normalize(bp);

		vec_t aDeep = aCenter     - normal * aRadius;
		vec_t bDeep = bCenterProj + normal * bRadius;

		return ManifoldPoints(aDeep, bDeep, normal, distance);
	}

	template<
		Dimension _d>
	ManifoldPoints Test_Capsule_Capsule(
		const ColliderBase* a, const Transform* at,
		const ColliderBase* b, const Transform* bt)
	{
		assert( a->Type == ColliderType::CAPSULE
			&& b->Type == ColliderType::CAPSULE
			&& a->Dim  == _d
			&& b->Dim  == _d);

		using Capsule = CapsuleCollider<_d>;

		Capsule* A = (Capsule*)a;
		Capsule* B = (Capsule*)b;

		return {};
	}

	// For meshes

	ManifoldPoints GetMaxPen(
		std::vector<ManifoldPoints>& manifolds)
	{
		if (manifolds.size() == 0) return {}; // exit if no collision

		size_t maxNormalIndex = 0;
		float  maxNormalDist  = FLT_MIN;

		for (size_t i = 0; i < manifolds.size(); i++) {
			if (manifolds[i].PenetrationDepth > maxNormalDist) {
				maxNormalDist = manifolds[i].PenetrationDepth;
				maxNormalIndex = i;
			}
		}

		return manifolds[maxNormalIndex];
	}

	template<
		Dimension _d>
	ManifoldPoints Test_X_Mesh(
		const ColliderBase* a, const Transform* at,
		const ColliderBase* b, const Transform* bt)
	{
		assert( a->Type != ColliderType::MESH
			&& b->Type == ColliderType::MESH
			&& a->Dim  == _d
			&& b->Dim  == _d);

		using Collider = Collider<_d>;
		using Mesh     = MeshCollider<_d>;

		Collider* A = (Collider*)a;
		Mesh*     B = (Mesh*)b;

		std::vector<ManifoldPoints> manifolds;

		for (Mesh::hull_t& part : B->GetHullParts())
		{
			if (!part.Bounds().Intersects(bt, A->Bounds(), at)) continue;

			auto [collision, simplex] = GJK(&part, bt, A, at);
			if (collision) {
 				manifolds.push_back(EPA(simplex, &part, bt, A, at));
			}
		}

		return GetMaxPen(manifolds);
	}

	template<
		Dimension _d>
	ManifoldPoints Test_Mesh_Mesh(
		const ColliderBase* a, const Transform* at,
		const ColliderBase* b, const Transform* bt)
	{
		assert( a->Type == ColliderType::MESH
			&& b->Type == ColliderType::MESH
			&& a->Dim  == _d
			&& b->Dim  == _d);

		using Mesh = MeshCollider<_d>;

		Mesh* A = (Mesh*)a;
		Mesh* B = (Mesh*)b;

		std::vector<ManifoldPoints> manifolds;

		for (Mesh::hull_t& aPart : A->GetHullParts())
		for (Mesh::hull_t& bPart : B->GetHullParts())
		{
			if (!aPart.Bounds().Intersects(at, bPart.Bounds(), bt)) continue;

			auto [collision, simplex] = GJK(&aPart, at, &bPart, bt);
			if (collision) {
				manifolds.push_back(EPA(simplex, &aPart, at, &bPart, bt));
			}
		}

		return GetMaxPen(manifolds);
	}

	template<
		Dimension _d>
	ManifoldPoints Test_GJK(
		const ColliderBase* a, const Transform* at,
		const ColliderBase* b, const Transform* bt)
	{
		assert( (a->Type == ColliderType::HULL || b->Type == ColliderType::HULL)
		     && (a->Type != ColliderType::MESH && b->Type != ColliderType::MESH)
			&& a->Dim == _d
			&& b->Dim == _d);

		using Collider = Collider<_d>;

		Collider* A = (Collider*)a;
		Collider* B = (Collider*)b;

		auto [collision, simplex] = GJK(A, at, B, bt);

		if (collision) {
			return EPA(simplex, A, at, B, bt);
		}

		return ManifoldPoints();
	}
}
}
}
