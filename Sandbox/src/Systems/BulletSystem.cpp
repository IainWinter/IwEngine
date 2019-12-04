#include "Systems/BulletSystem.h"
#include "iw/engine/Time.h"

struct Components {
	IW::Transform* Transform;
	Bullet*              Bullet;
};

BulletSystem::BulletSystem()
	: IW::System<IW::Transform, Bullet>("Bullet")
{}

BulletSystem::~BulletSystem()
{

}

void BulletSystem::Update(
	IW::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, bullet] = entity.Components.Tie<Components>();

		if (bullet->Type == LINE) {
			transform->Position += iw::vector3(1, 0, 0) * transform->Rotation * bullet->Speed * IW::Time::DeltaTime();
		}

		if (transform->Position.x > 32.25f || transform->Position.x < -32.25f || transform->Position.z > 18.25f || transform->Position.z < -18.25f) {
			QueueDestroyEntity(entity.Index);
		}
	}
}
