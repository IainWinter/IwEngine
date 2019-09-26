#include "Systems/BulletSystem.h"
#include "iw/engine/Time.h"

struct Components {
	IwEngine::Transform* Transform;
	Bullet*              Bullet;
};

BulletSystem::BulletSystem(
	IwEntity::Space& space,
	IwGraphics::RenderQueue& renderQueue)
	: IwEngine::System<IwEngine::Transform, Bullet>(space, renderQueue, "Bullet")
{}

BulletSystem::~BulletSystem()
{

}

void BulletSystem::Update(
	View& view)
{
	for (auto entity : view) {
		auto [transform, bullet] = entity->Components->Tie<Components>();

		if (bullet->Type == LINE) {
			transform->Position += iwm::vector3(1, 1, 0) * transform->Rotation * bullet->Speed * IwEngine::Time::DeltaTime();
		}

		if (transform->Position.x > 65 || transform->Position.x < -65 || transform->Position.y > 36 || transform->Position.y < -36) {
			QueueDestroyEntity(entity->Index);
		}
	}
}
