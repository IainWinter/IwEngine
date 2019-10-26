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

		if (transform->Position.x > 31 || transform->Position.x < -31 || transform->Position.y > 17 || transform->Position.y < -17) {
			QueueDestroyEntity(entity.Index);
		}
	}
}
