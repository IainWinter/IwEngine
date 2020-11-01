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
	iw::Entity& player)
	: iw::SystemBase("Special Barrier")
	, player(player)
	, dashThrough(false)
{}

// sucks that this needs to be here, need to fix lambdas to get it to work
void SpecialBarrierSystem::collide(
	iw::Manifold& man,
	iw::scalar dt)
{
	iw::Entity barrierEntity, playerEntity;
	if (GetEntitiesFromManifold<DontDeleteBullets, Player>(man, barrierEntity, playerEntity)) {
		return;
	}

	if (man.PenetrationDepth > 0.5f) {
		man.HasCollision = false;

		if (   !dashThrough
			&& playerEntity.Find<Player>()->Timer <= 0)
		{
			iw::Rigidbody* body = playerEntity.Find<iw::Rigidbody>();

			body->SetSimGravity(true);
			body->SetIsLocked(0);
			body->SetCol(nullptr);

			playerEntity.Find<Player>()->Speed = 0;

			float start = iw::TotalTime();
			float wait = 2.00f;

			Task->queue([=]() {
				while (iw::TotalTime() - start < wait) {
					// spin lock
				}

				Bus->push<ResetLevelEvent>();
			});
		}
	}
}

void SpecialBarrierSystem::Update() {
	auto query = Space->MakeQuery<iw::CollisionObject, DontDeleteBullets>();
	query->SetNone({ Space->GetComponent<WorldHole>() });

	for(auto entity : Space->Query(query)) {
		iw::CollisionObject* object = entity.Components.Get<iw::CollisionObject, 0>();

		if (   dashThrough
			&& player.Find<Player>()->Timer > 0.0f)
		{
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
			StartLevelEvent& event = e.as<StartLevelEvent>();

			if (event.LevelName == "levels/canyon/cave04.json") {
				event.Level->AddChild(SpawnSpecialBarrier({iw::vector3(0.6f, 0, -0.2f), iw::vector3::unit_x, 29.8f, 2.2f }));
			}

			else if (event.LevelName == "levels/canyon/cave06.json") {
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(6.4f, 0, -4.4f), iw::vector3::unit_x, 60, 2 }));
			}

			else if (event.LevelName == "levels/canyon/cave07.json") {
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 2.2f, 0, -1.6f),  iw::vector3::unit_x,  6.7f, 0.3f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 2.7f, 0,  1.6f),  iw::vector3::unit_x,  7.4f, 0.3f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 0.5f, 0, 14   ),  iw::vector3::unit_z, 23.8f, 9    }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(10.7f, 0,  10.9f), iw::vector3::unit_x, 24.4f, 5.8f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 6.1f, 0,  3.5f),  iw::vector3::unit_z,  4,    0.3f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 5.3f, 0, -6.6f),  iw::vector3::unit_z, 10.6f, 0.3f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3(-1,    0, -6.6f),  iw::vector3::unit_z, 10.6f, 0.3f }));
				event.Level->AddChild(SpawnSpecialBarrier({ iw::vector3( 2.1,  0, -6.6f),  iw::vector3::unit_z, 10.0f, 2.7f }));
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
