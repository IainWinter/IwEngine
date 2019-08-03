#include "BulletSystem.h"
#include "iw/engine/Time.h"

BulletSystem::BulletSystem(
	IwEntity::Space& space)
	: IwEngine::System<IwEngine::Transform, Bullet>(space, "Player")
{}

BulletSystem::~BulletSystem()
{

}

void BulletSystem::Update(
	IwEntity::View<IwEngine::Transform, Bullet>& view)
{
	for (auto components : view) {
		auto& transform = components.GetComponent<IwEngine::Transform>();
		auto& bullet    = components.GetComponent<Bullet>();

		if (bullet.Type == LINE) {
			transform.Position += iwm::vector3(1, 1, 0) * transform.Rotation * bullet.Speed * IwEngine::Time::DeltaTime();
		}

		if (transform.Position.x > 65 || transform.Position.x < -65 || transform.Position.y > 36 || transform.Position.y < -36) {
			QueueDestroyEntity(components.Entity);
		}
	}
}
