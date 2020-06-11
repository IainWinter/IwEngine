#include "Systems/BulletSystem.h"
#include "Events/ActionEvents.h"
#include "iw/engine/Time.h"

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

		//if (bullet->Die) {
		//	rigidbody->Trans().Scale -= rigidbody->Trans().Scale * iw::Time::DeltaTime() * 2;
		//	if (rigidbody->Trans().Scale.x < 0.0f) {
		//		transform->SetParent(nullptr);
		//		Space->DestroyEntity(entity.Index);
		//	}
		//}

		switch (bullet->Type) {
			case SINE: {
				float speed = (sin(bullet->Time * 5) + 1) * 0.5f;
				rigidbody->SetVelocity(bullet->initialVelocity * speed);

				break;
			}
			case ORBIT: {
				iw::vector3 target = player.Find<iw::Transform>()->Position;
				rigidbody->ApplyForce((target - transform->Position) * 0.5f);

				break;
			}
			case SEEK: {
				iw::vector3 target = player.Find<iw::Transform>()->Position;
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
