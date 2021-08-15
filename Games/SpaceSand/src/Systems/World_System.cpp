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
				}

				iw::Cell c;
				c.Type = iw::CellType::STONE; // temp
				c.Color = iw::Color(0, 1, 0);

				for (int y = 0; y < tile->m_sprite.Height(); y++)
				for (int x = 0; x < tile->m_sprite.Width(); x++)
				{
					if (iw::Color::From32(tile->m_sprite.Colors32()[x + y * tile->m_sprite.Width()]).a != 0)
					{
						sand->m_world->SetCell(transform->Position.x + x, transform->Position.y + y, c);
					}
				}

				Space->QueueEntity(entity, iw::func_Destroy);
			}
		});
}
