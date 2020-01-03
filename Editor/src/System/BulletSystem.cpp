#include "Systems/BulletSystem.h"
#include "iw/engine/Time.h"

BulletSystem::BulletSystem()
	: IW::System<IW::Transform, IW::Rigidbody, Bullet>("Bullet")
{}

BulletSystem::~BulletSystem() {}

void BulletSystem::Update(
	IW::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, rigidbody, bullet] = entity.Components.Tie<Components>();

		if (bullet->Type == LINE) {
			//rigidbody->MovePosition(transform->Position + iw::vector3(1, 0, 0) * transform->Rotation * bullet->Speed * IW::Time::FixedTime());
		}

		if (bullet->Time > 5.f) {
			QueueDestroyEntity(entity.Index);
			Physics->RemoveRigidbody(rigidbody);
		}

		bullet->Time += IW::Time::DeltaTime();
	}
}

bool BulletSystem::On(
	IW::CollisionEvent& e)
{
	LOG_INFO << e.PenetrationDepth;

	return false;
}
