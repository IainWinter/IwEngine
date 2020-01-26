#include "Systems/BulletSystem.h"
#include "iw/engine/Time.h"

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

		transform->Position.y = 1;

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

	if (   a.Index() != IW::EntityHandle::Empty.Index 
		&& a.HasComponent<Bullet>() 
		&& a.HasComponent<IW::Rigidbody>())
	{
		Physics->RemoveRigidbody(a.FindComponent<IW::Rigidbody>());
		QueueDestroyEntity(a.Index());
	}

	if (   b.Index() != IW::EntityHandle::Empty.Index 
		&& b.HasComponent<Bullet>() 
		&& b.HasComponent<IW::Rigidbody>())
	{
		Physics->RemoveRigidbody(b.FindComponent<IW::Rigidbody>());
		QueueDestroyEntity(b.Index());
	}

	return false;
}
