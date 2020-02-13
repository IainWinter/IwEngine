#include "Systems/BulletSystem.h"
#include "iw/engine/Time.h"
#include <Components\Enemy.h>

BulletSystem::BulletSystem()
	: iw::System<iw::Transform, iw::Rigidbody, Bullet>("Bullet")
{}

void BulletSystem::Update(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, rigidbody, bullet] = entity.Components.Tie<Components>();

		if (bullet->Type == LINE) {
			//rigidbody->MovePosition(transform->Position + iw::vector3(1, 0, 0) * transform->Rotation * bullet->Speed * iw::Time::FixedTime());
		}

		/*if (bullet->Time > 5.f) {
			QueueDestroyEntity(entity.Index);
			Physics->RemoveRigidbody(rigidbody);
		}*/

		bullet->Time += iw::Time::DeltaTime();
	}
}

bool BulletSystem::On(
	iw::CollisionEvent& event)
{
	iw::Entity a = Space->FindEntity(event.BodyA);
	iw::Entity b = Space->FindEntity(event.BodyB);

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

	if (    other != iw::EntityHandle::Empty
		&& (other.HasComponent<Enemy>() || other.HasComponent<Bullet>()))
	{
		return false;
	}

	if (bullet != iw::EntityHandle::Empty) {
		QueueDestroyEntity(bullet.Index());
	}

	return false;
}
