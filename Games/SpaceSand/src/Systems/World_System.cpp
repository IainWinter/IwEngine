#include "Systems/World_System.h"

int WorldSystem::Initialize()
{
	m_timer.SetTime("spawn_enemy", 2);

	return 0;
}

bool once = true;

void WorldSystem::FixedUpdate()
{
	m_timer.TickFixed();

	if(once)
	if (m_timer.Can("spawn_enemy"))
	{
		iw::Entity e = sand->MakeTile("textures/SpaceGame/enemy.png", true);
		AddComponentToPhysicsEntity<EnemyShip>(e);
		AddComponentToPhysicsEntity<Flocker>(e);

		iw::Transform* t = e.Set<iw::Transform>();
		iw::Rigidbody* r = e.Set<iw::Rigidbody>();

		EnemyShip* s = e.Set<EnemyShip>();
		Flocker*   f = e.Set<Flocker>();

		auto [w, h] = sand->GetSandTexSize2();
		float margin = .1f;

		f->Target.x = iw::randi(w - w * margin * 2) + w * margin;
		f->Target.y = iw::randi(h - h * margin * 2) + h * margin;

		once = false;
	}

	Space->Query<iw::Tile>().Each(
		[&](
			iw::EntityHandle entity, 
			iw::Tile* tile) 
		{
			if (tile->m_currentCellCount < tile->m_initalCellCount / 3)
			{
				Space->QueueEntity(entity, iw::func_Destroy);
			}

			LOG_INFO << tile << " " << tile->m_currentCellCount;
		}
	);
}
