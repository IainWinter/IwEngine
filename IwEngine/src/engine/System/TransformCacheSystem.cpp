#include "iw/engine/Systems/TransformCacheSystem.h"

namespace iw {
namespace Engine {
	void TransformCacheSystem::Update() 
	{
		Space->Query<iw::Transform>().Each([](iw::EntityHandle, iw::Transform* transform) {
			transform->CacheTransformation();
		});
	}

	void TransformCacheSystem::FixedUpdate()
	{
		Space->Query<iw::MeshCollider>().Each([](iw::EntityHandle, iw::MeshCollider* mesh) {
			mesh->CalcHullParts();
		});

		Space->Query<iw::MeshCollider2>().Each([](iw::EntityHandle, iw::MeshCollider2* mesh) {
			mesh->CalcHullParts();
		});

		for (CollisionObject* object : Physics->CollisionObjects())
		{
			(iw::impl::Collider<d2>*)object->Collider
		}
	}
}
}
