#include "Systems/BulletSystem.h"
#include "Events/ActionEvents.h"
#include "Components/Player.h"
#include "iw/engine/Time.h"
#include <Components\Enemy.h>

struct PlayerComponents {
	iw::Transform* Transform;
	Player* Player;
};

BulletSystem::BulletSystem(
	iw::Entity& player)
	: iw::System<iw::Transform, iw::Rigidbody, Bullet>("Bullet")
	, player(player)
{}

void BulletSystem::FixedUpdate(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, rigidbody, bullet] = entity.Components.Tie<Components>();

		switch (bullet->Type) {
			case SINE: {
				float speed = (sin(bullet->Time * 5) + 1) * 0.5f;
				rigidbody->SetVelocity(bullet->initialVelocity * speed);

				break;
			}
			case ORBIT: {
				iw::vector3 target = player.FindComponent<iw::Transform>()->Position;
				rigidbody->ApplyForce((target - transform->Position) * 0.5f);

				break;
			}
			case SEEK: {
				iw::vector3 target = player.FindComponent<iw::Transform>()->Position;
				rigidbody->SetVelocity((target - transform->Position).normalized() * bullet->Speed);

				break;
			}
		}

		bullet->Time += iw::Time::FixedTime();

		/*if (bullet->Time > 5.f) {
			QueueDestroyEntity(entity.Index);
			Physics->RemoveRigidbody(rigidbody);
		}*/
	}
}

bool BulletSystem::On(
	iw::CollisionEvent& event)
{
	iw::Entity a = Space->FindEntity(event.ObjA);
	if (a == iw::EntityHandle::Empty) {
		a = Space->FindEntity<iw::Rigidbody>(event.ObjA);
	}

	iw::Entity b = Space->FindEntity(event.ObjB);
	if (b == iw::EntityHandle::Empty) {
		b = Space->FindEntity<iw::Rigidbody>(event.ObjB);
	}

	iw::Entity bullet;
	iw::Entity other;
	if (   a != iw::EntityHandle::Empty 
		&& a.HasComponent<Bullet>()) 
	{
		bullet = a;
		other  = b;
	}

	else if (b != iw::EntityHandle::Empty
		  && b.HasComponent<Bullet>())
	{
		bullet = b;
		other  = a;
	}

	if (   other != iw::EntityHandle::Empty
		&& (   other.HasComponent<Bullet>() 
			|| other.Index() == bullet.FindComponent<Bullet>()->enemyIndex))
	{
		return false;
	}

	if (bullet != iw::EntityHandle::Empty) {
		bullet.FindComponent<iw::Transform>()->SetParent(nullptr);
		Space->DestroyEntity(bullet.Index());
	}

	return false;
}

bool BulletSystem::On(
	iw::EntityDestroyedEvent& e)
{
	if (e.Entity.HasComponent<Enemy>()) {
		iw::vector3 position = e.Entity.FindComponent<iw::Transform>()->Position;

		for (auto entity : Space->Query<iw::Transform, Bullet>()) {
			auto [transform, bullet] = entity.Components.Tie<BulletComponents>();

			float distance = (transform->Position - position).length();
			if (distance < 4) {
				Space->DestroyEntity(entity.Index);
			}
		}
	}

	return false;
}
