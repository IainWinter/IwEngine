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

	if (once)
	if (m_timer.Can("spawn_enemy"))
	{
		auto [w, h] = sand->GetSandTexSize();
		float margin = .1f;

		float target_x = iw::randi(w - w * margin * 2) + w * margin;
		float target_y = iw::randi(h - h * margin * 2) + h * margin;

		Bus->push<SpawnEnemy_Event>(m_player, target_x, target_y);

		once = false;
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
					return;
				}

				//int amount = iw::randi(6);
				//for (int i = 0; i < amount; i++)
				//{
				//	iw::Entity health = sand->MakeTile("textures/SpaceGame/health.png", true);

				//	health.Find<iw::Rigidbody>()->Transform.Position.x = transform->Position.x + iw::randi(10);
				//	health.Find<iw::Rigidbody>()->Transform.Position.y = transform->Position.y + iw::randi(10);
				//}

				iw::Cell c;
				c.Type = iw::CellType::STONE; // temp
				c.Color = iw::Color(0, 1, 0);
				c.Props = iw::CellProp::MOVE_FORCE;
				
				for (int y = 0; y < tile->m_sprite.Height(); y++)
				for (int x = 0; x < tile->m_sprite.Width(); x++)
				{
					if (iw::Color::From32(tile->m_sprite.Colors32()[x + y * tile->m_sprite.Width()]).a != 0)
					{
						sand->m_world->SetCell(transform->Position.x + x - tile->m_sprite.Width() / 2, transform->Position.y + y - tile->m_sprite.Height() / 2, c);
					}
				}

				Space->QueueEntity(entity, iw::func_Destroy);
			}
		});
}
