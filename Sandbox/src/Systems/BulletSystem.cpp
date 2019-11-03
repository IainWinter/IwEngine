#include "Systems/BulletSystem.h"
#include "iw/engine/Time.h"

struct Components {
	IW::Transform* Transform;
	Bullet*              Bullet;
};

BulletSystem::BulletSystem(
	IwEntity::Space& space,
	IW::Graphics::Renderer& renderer)
	: IwEngine::System<IW::Transform, Bullet>(space, renderer, "Bullet")
{}

BulletSystem::~BulletSystem()
{

}

void BulletSystem::Update(
	IwEntity::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, bullet] = entity.Components.Tie<Components>();

		if (bullet->Type == LINE) {
			transform->Position += iwm::vector3(1, 0, 0) * transform->Rotation * bullet->Speed * IwEngine::Time::DeltaTime();
		}

		if (transform->Position.x > 32.25f || transform->Position.x < -32.25f || transform->Position.z > 18.25f || transform->Position.z < -18.25f) {
			QueueDestroyEntity(entity.Index);
		}
	}
}
