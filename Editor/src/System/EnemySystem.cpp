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
	iw::ref<IW::Model> bulletModel)
	: IW::System<IW::Transform, Enemy>("Enemy")
	, BulletModel(bulletModel)
{}

void EnemySystem::Update(
	IW::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, enemy] = entity.Components.Tie<Components>();

		transform->Rotation *= iw::quaternion::from_euler_angles(0, -IW::Time::DeltaTime(), 0);

		if (  !enemy->HasShot
			&& enemy->Timer <= 0)
		{
			enemy->HasShot = true;

			enemy->Rotation = fmod(enemy->Rotation + enemy->Speed, iw::PI2);
			iw::quaternion rot = iw::quaternion::from_euler_angles(0, enemy->Rotation, 0);

			iw::vector3 v = iw::vector3::unit_x * rot;
			v.normalize();
			v *= 5;

			IW::Entity bullet = Space->CreateEntity<IW::Transform, IW::Model, IW::SphereCollider, IW::Rigidbody, Bullet>();
			bullet.SetComponent<IW::Model>(*BulletModel);
			bullet.SetComponent<Bullet>   (LINE, 5.0f);

			IW::Transform* t      = bullet.SetComponent<IW::Transform>     (transform->Position + iw::vector3(sqrt(2), 0, 0) * rot, iw::vector3(.25f));
			IW::SphereCollider* s = bullet.SetComponent<IW::SphereCollider>(iw::vector3::zero, .25f);
			IW::Rigidbody* r      = bullet.SetComponent<IW::Rigidbody>     ();

			r->SetMass(1);
			r->SetCol(s);
			r->SetTrans(t);
			r->SetVelocity(v);
			r->SetSimGravity(false);
			r->SetRestitution(1);
			r->SetIsTrigger(true);

			Physics->AddRigidbody(r);
		}

		if (enemy->Timer <= -enemy->CooldownTime) {
			enemy->HasShot = false;
			enemy->Timer = enemy->FireTime;
		}

		if (enemy->Timer >= -enemy->CooldownTime) {
			enemy->Timer -= IW::Time::DeltaTime();
		}
	}
}

bool EnemySystem::On(
	IW::CollisionEvent& event)
{
	IW::Entity a = Space->FindEntity(event.BodyA);
	IW::Entity b = Space->FindEntity(event.BodyB);

	if (   a.Index() == IW::EntityHandle::Empty.Index
		|| b.Index() == IW::EntityHandle::Empty.Index)
	{
		return false;
	}

	IW::Entity player;
	IW::Entity enemy;
	if (a.HasComponent<Player>() && b.HasComponent<Enemy>()) {
		player = a;
		enemy  = b;
	}

	else if (b.HasComponent<Player>() && a.HasComponent<Enemy>()) {
		player = b;
		enemy  = a;
	}

	if (   player.Index() != IW::EntityHandle::Empty.Index
		|| enemy .Index() != IW::EntityHandle::Empty.Index)
	{
		Player* playerComponent = player.FindComponent<Player>();
		if (playerComponent->Timer > 0) {
			QueueDestroyEntity(enemy.Index());
			Physics->RemoveRigidbody(enemy.FindComponent<IW::Rigidbody>());
		}

		else {
			playerComponent->Health -= 1;
		}
	}

	return false;
}
