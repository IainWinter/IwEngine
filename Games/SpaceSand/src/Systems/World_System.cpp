#include "Systems/World_System.h"

int WorldSystem::Initialize()
{
	auto [w, h] = sand->GetSandTexSize2();
	sand->SetCamera(w, h, 10, 10);

	m_timer.SetTime("spawn_enemy", 2);
	return 0;
}

bool once = true;

void WorldSystem::FixedUpdate()
{
	m_timer.TickFixed();

	//if (m_timer.Can("spawn_enemy"))
	//{
	//	auto [w, h] = sand->GetSandTexSize();
	//	float margin = .1f;

	//	float target_x = iw::randi(w - w * margin * 2) + w * margin;
	//	float target_y = iw::randi(h - h * margin * 2) + h * margin;

	//	Bus->push<SpawnEnemy_Event>(m_player, target_x, target_y);
	//}

	Space->Query<iw::Transform, iw::Tile>().Each(
		[&](
			iw::EntityHandle entity, 
			iw::Transform* transform,
			iw::Tile* tile) 
		{

			if (tile->m_currentCellCount < tile->m_initalCellCount / 3)
			{
				if (Space->HasComponent<Player>(entity))
				{
					//reset();
					return;
				}

				int amount = iw::randi(5) + 1;
				Bus->push<SpawnHealth_Event>(transform->Position.x, transform->Position.y, amount);

				Space->QueueEntity(entity, iw::func_Destroy);
			}
		});


	if (iw::Mouse::ButtonDown(iw::MMOUSE))
	{
		Bus->push<SpawnHealth_Event>(sand->sP.x, sand->sP.y, 1);
	}
}
