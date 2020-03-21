#include "Systems/BulletSystem.h"
#include "Events/ActionEvents.h"
#include "Components/Player.h"
#include "iw/engine/Time.h"
#include <Components\Enemy.h>

struct PlayerComponents {
	iw::Transform* Transform;
	Player* Player;
};

BulletSystem::BulletSystem()
	: iw::System<iw::Transform, iw::Rigidbody, Bullet>("Bullet")
{}

void BulletSystem::FixedUpdate(
	iw::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, rigidbody, bullet] = entity.Components.Tie<Components>();

		switch (bullet->Type) {
			case SINE: {
				rigidbody->SetVelocity(bullet->initialVelocity * (1 + sin(bullet->Time * 5)) * 0.5f);
				break;
			}
			case SEEK: {
				iw::vector3 target;
				float maxDistance = -1;
				for (auto entity : Space->Query<iw::Transform, Player>()) {
					auto [pt, p] = entity.Components.Tie<PlayerComponents>();

					float distance = (pt->Position - transform->Position).length();
					if (distance > maxDistance) {
						target = pt->Position;
						maxDistance = distance;
					}
				}

				if (maxDistance != -1) {
					rigidbody->ApplyForce((target - transform->Position) * 0.5f);
				}

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

	if (    other != iw::EntityHandle::Empty
		&& (other.HasComponent<Enemy>() || other.HasComponent<Bullet>()))
	{
		return false;
	}

	if (bullet != iw::EntityHandle::Empty) {
		bullet.FindComponent<iw::Transform>()->SetParent(nullptr);
		Space->DestroyEntity(bullet.Index());
	}

	return false;
}
