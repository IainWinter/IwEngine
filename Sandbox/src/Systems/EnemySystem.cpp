#include "Systems/EnemySystem.h"
#include "Components/Bullet.h"
#include "iw/engine/Time.h"
#include "iw/engine/Components/Model.h"
#include "iw/input/Devices/Keyboard.h"
#include "iw/physics/AABB.h"

#include "IW/physics/AABB.h";

EnemySystem::EnemySystem(
	IW::Mesh* circle)
	: IW::System<IW::Transform, Enemy>("Enemy")
	, CircleMesh(circle)
{}

void EnemySystem::Destroy() {
	CircleMesh->Destroy(Renderer->Device);
}

void EnemySystem::Update(
	IW::EntityComponentArray& view)
{
	for (auto entity : view) {
		auto [transform, enemy] = entity.Components.Tie<Components>();

		if (  !enemy->HasShot
			&& enemy->Timer <= 0)
		{
			enemy->HasShot = true;

			enemy->Rotation = fmod(enemy->Rotation + enemy->Speed, iw::PI2);
			iw::quaternion rot = iw::quaternion::from_euler_angles(0, enemy->Rotation, 0);

			IW::Entity spawned = Space->CreateEntity<IW::Transform, IW::ModelComponent, Bullet, IW::AABB2>();
			
			Space->SetComponentData<IW::Transform>     (spawned, transform->Position + iw::vector3(sqrt(2), 0, 0) * rot, iw::vector3(.25f), rot);
			Space->SetComponentData<IW::ModelComponent>(spawned, CircleMesh, 1U);
			Space->SetComponentData<Bullet>            (spawned, LINE, 5.0f);
			Space->SetComponentData<IW::AABB2> (spawned, iw::vector2(-0.25f), iw::vector2(0.25f));
		}

		if (enemy->Timer <= -enemy->CooldownTime) {
			enemy->HasShot = false;
			enemy->Timer = enemy->FireTime;
		}

		if (enemy->Timer >= -enemy->CooldownTime) {
			enemy->Timer -= IW::Time::DeltaTime();
		}
	}
}
