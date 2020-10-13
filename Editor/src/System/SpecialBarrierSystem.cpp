#include "Systems/SpecialBarrierSystem.h"

#include "Components/DontDeleteBullets.h"
#include "Components/WorldHole.h"
#include "Components/Player.h"
#include "Events/ActionEvents.h"

#include "iw/common/Components/Transform.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/graphics/Model.h"
#include "iw/events/binding.h"

SpecialBarrierSystem::SpecialBarrierSystem(
	iw::Entity& currentLevel,
	iw::Entity& player)
	: iw::SystemBase("Special Barrier")
	, currentLevel(currentLevel)
	, player(player)
	, dashThrough(false)
{}

// sucks that this needs to be here, need to fix lambdas to get it to work
void SpecialBarrierSystem::collide(iw::Manifold& man, iw::scalar dt)
{
	iw::Entity barrierEntity, playerEntity;
	bool noent = GetEntitiesFromManifold<DontDeleteBullets, Player>(man, barrierEntity, playerEntity);

	if (noent) {
		return;
	}

	if (dashThrough && playerEntity.Find<Player>()->Timer <= 0 && man.PenetrationDepth > 0.25f) {
		Bus->push<ResetLevelEvent>();
	}
}

int SpecialBarrierSystem::Initialize() {
	return 0;
}

void SpecialBarrierSystem::Update() {
	auto query = Space->MakeQuery<iw::CollisionObject, DontDeleteBullets>();
	query->SetNone({ Space->GetComponent<WorldHole>() });

	for(auto entity : Space->Query(query)) {
		iw::CollisionObject* object = entity.Components.Get<iw::CollisionObject, 0>();

		if (dashThrough && player.Find<Player>()->Timer > 0.0f) {
			object->SetIsTrigger(true);
		}

		else {
			object->SetIsTrigger(false);
		}
	}
}

bool SpecialBarrierSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::LONG_DASH_ACTIVE): {
			dashThrough = e.as<LongDashEvent>().Active;

			break;
		}
		case iw::val(Actions::START_LEVEL): {
			std::string name = e.as<StartLevelEvent>().LevelName;

			std::vector<iw::CapsuleCollider> colliders;

			if (name == "levels/canyon/cave04.json") {
				colliders.push_back({ iw::vector3(0.6f, 0, -0.2f), iw::vector3::unit_x, 29.8f, 2.2f });
			}

			else if (name == "levels/canyon/cave06.json") {
				colliders.push_back({ iw::vector3(6.4f, 0, -4.4f), iw::vector3::unit_x, 60, 2 });
			}

			else if (name == "levels/canyon/cave07.json") {
				colliders.push_back({ iw::vector3( 2.2f, 0, -1.6f), iw::vector3::unit_x,  6.7f, 0.3f });
				colliders.push_back({ iw::vector3( 2.7f, 0,  1.6f), iw::vector3::unit_x,  7.4f, 0.3f });
				colliders.push_back({ iw::vector3( 0.5f, 0,  14 ), iw::vector3::unit_z, 23.8f, 9   });
				colliders.push_back({ iw::vector3(11.5f, 0,  5.1f), iw::vector3::unit_x, 11.7f, 0.3f });
				colliders.push_back({ iw::vector3( 6.1f, 0,  3.5f), iw::vector3::unit_z,  4,   0.3f });
				colliders.push_back({ iw::vector3( 5.3f, 0, -6.6f), iw::vector3::unit_z, 10.6f, 0.3f });
				colliders.push_back({ iw::vector3(-1,   0, -6.6f), iw::vector3::unit_z, 10.6f, 0.3f });
			}		

			for (iw::CapsuleCollider& collider : colliders) {
				iw::Transform* transform = SpawnSpecialBarrier(collider);
				currentLevel.Find<iw::Transform>()->AddChild(transform);
			}

			break;
		}
	}

	return false;
}

iw::Transform* SpecialBarrierSystem::SpawnSpecialBarrier(
	iw::CapsuleCollider capsule)
{
	iw::Entity ent = Space->CreateEntity<iw::Transform, iw::CapsuleCollider, iw::CollisionObject, DontDeleteBullets>();

	iw::Transform*       transform = ent.Set<iw::Transform>(iw::vector3::unit_y);
	iw::CapsuleCollider* collider  = ent.Set<iw::CapsuleCollider>(capsule);
	iw::CollisionObject* object    = ent.Set<iw::CollisionObject>();

	object->SetCol(collider);
	object->SetTrans(transform);

	object->SetOnCollision(iw::bind<void, SpecialBarrierSystem*, iw::Manifold&, float>(&SpecialBarrierSystem::collide, this)); // garbo

	Physics->AddCollisionObject(object);

	return transform;
}
