#include "Systems/World_System.h"

int WorldSystem::Initialize()
{
	m_timer.SetTime("spawn_enemy", 2);

	return 0;
}

void WorldSystem::FixedUpdate()
{
	m_timer.TickFixed();

	if (m_timer.Can("spawn_enemy"))
	{
		auto [w, h] = sand->GetSandTexSize();
		float margin = .1f;

		float target_x = iw::randi(w - w * margin * 2) + w * margin;
		float target_y = iw::randi(h - h * margin * 2) + h * margin;

		Bus->push<SpawnEnemy_Event>(m_player, target_x, target_y);
	}

	Space->Query<iw::Tile>().Each(
		[&](
			iw::EntityHandle entity, 
			iw::Tile* tile) 
		{
			if (tile->m_currentCellCount < tile->m_initalCellCount / 3)
			{
				if (Space->HasComponent<Player>(entity))
				{
					//reset();
				}

				Space->QueueEntity(entity, iw::func_Destroy);
			}
		});
}
