#include "Systems/BulletSystem.h"
#include "iw/engine/Time.h"
#include <Components\Enemy.h>

BulletSystem::BulletSystem()
	: IW::System<IW::Transform, IW::Rigidbody, Bullet>("Bullet")
{}

void BulletSystem::Update(
	IW::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, rigidbody, bullet] = entity.Components.Tie<Components>();

		if (bullet->Type == LINE) {
			//rigidbody->MovePosition(transform->Position + iw::vector3(1, 0, 0) * transform->Rotation * bullet->Speed * IW::Time::FixedTime());
		}

		/*if (bullet->Time > 5.f) {
			QueueDestroyEntity(entity.Index);
			Physics->RemoveRigidbody(rigidbody);
		}*/

		bullet->Time += IW::Time::DeltaTime();
	}
}

bool BulletSystem::On(
	IW::CollisionEvent& event)
{
	IW::Entity a = Space->FindEntity(event.BodyA);
	IW::Entity b = Space->FindEntity(event.BodyB);

	IW::Entity bullet;
	IW::Entity other;
	if (   a != IW::EntityHandle::Empty 
		&& a.HasComponent<Bullet>()) 
	{
		bullet = a;
		other  = b;
	}

	else if (b != IW::EntityHandle::Empty
		  && b.HasComponent<Bullet>())
	{
		bullet = b;
		other  = a;
	}

	if (    other != IW::EntityHandle::Empty
		&& (other.HasComponent<Enemy>() || other.HasComponent<Bullet>()))
	{
		return false;
	}

	if (bullet != IW::EntityHandle::Empty) {
		Physics->RemoveRigidbody(bullet.FindComponent<IW::Rigidbody>());
		QueueDestroyEntity(bullet.Index());
	}

	return false;
}
