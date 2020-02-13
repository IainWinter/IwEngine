#include "Systems/EnemySystem.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/graphics/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/physics/AABB.h"

#include "IW/physics/Collision/SphereCollider.h";
#include "IW/physics/Dynamics/Rigidbody.h";
#include <Components\Player.h>

EnemySystem::EnemySystem(
	iw::ref<iw::Model> bulletModel)
	: iw::System<iw::Transform, Enemy>("Enemy")
	, BulletModel(bulletModel)
{}

void EnemySystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, enemy] = entity.Components.Tie<Components>();

		transform->Rotation *= iw::quaternion::from_euler_angles(0, -iw::Time::DeltaTime(), 0);

		if (  !enemy->HasShot
			&& enemy->Timer <= 0)
		{
			enemy->HasShot = true;

			enemy->Rotation = fmod(enemy->Rotation + enemy->Speed, iw::Pi2);
			iw::quaternion rot = iw::quaternion::from_euler_angles(0, enemy->Rotation, 0);

			iw::vector3 v = iw::vector3::unit_x * rot;
			v.normalize();
			v *= 5;

			iw::Entity bullet = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::Rigidbody, Bullet>();
			bullet.SetComponent<iw::Model>(*BulletModel);
			bullet.SetComponent<Bullet>   (LINE, 5.0f);

			iw::Transform* t      = bullet.SetComponent<iw::Transform>     (transform->Position + iw::vector3(sqrt(2), 0, 0) * rot, iw::vector3(.25f));
			iw::SphereCollider* s = bullet.SetComponent<iw::SphereCollider>(iw::vector3::zero, .25f);
			iw::Rigidbody* r      = bullet.SetComponent<iw::Rigidbody>     ();

			r->SetMass(1);
			r->SetCol(s);
			r->SetTrans(t);
			r->SetVelocity(v);
			r->SetSimGravity(false);
			r->SetRestitution(1);
			r->SetIsTrigger(true);

			r->SetIsLocked(iw::vector3(0, 1, 0));
			r->SetLock(iw::vector3(0, 1, 0));

			Physics->AddRigidbody(r);
		}

		if (enemy->Timer <= -enemy->CooldownTime) {
			enemy->HasShot = false;
			enemy->Timer = enemy->FireTime;
		}

		if (enemy->Timer >= -enemy->CooldownTime) {
			enemy->Timer -= iw::Time::DeltaTime();
		}
	}
}

bool EnemySystem::On(
	iw::CollisionEvent& event)
{
	iw::Entity a = Space->FindEntity(event.BodyA);
	iw::Entity b = Space->FindEntity(event.BodyB);

	if (   a.Index() == iw::EntityHandle::Empty.Index
		|| b.Index() == iw::EntityHandle::Empty.Index)
	{
		return false;
	}

	iw::Entity player;
	iw::Entity enemy;
	if (a.HasComponent<Player>() && b.HasComponent<Enemy>()) {
		player = a;
		enemy  = b;
	}

	else if (b.HasComponent<Player>() && a.HasComponent<Enemy>()) {
		player = b;
		enemy  = a;
	}

	if (   player.Index() != iw::EntityHandle::Empty.Index
		|| enemy .Index() != iw::EntityHandle::Empty.Index)
	{
		Player* playerComponent = player.FindComponent<Player>();
		if (playerComponent->Timer > 0) {
			QueueDestroyEntity(enemy.Index());
		}

		else {
			playerComponent->Health -= 1;
		}
	}

	return false;
}
