#include "Systems/World_System.h"

int WorldSystem::Initialize()
{
	auto [w, h] = sand->GetSandTexSize2();
	sand->SetCamera(w, h, 10, 10);

	m_timer.SetTime("spawn_enemy", 2);
	return 0;
}

bool once = true;
int x = 0;

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
					return;
				}

				SpawnItem_Event::IType itemType;
				if (iw::randf() > .5) itemType = SpawnItem_Event::LASER_CHARGE;
				else                  itemType = SpawnItem_Event::HEALTH;

				int amount = iw::randi(5) + 1;
				Bus->push<SpawnItem_Event>(transform->Position.x, transform->Position.y, amount, itemType);

				Space->QueueEntity(entity, iw::func_Destroy);
			}
		});


	if (iw::Mouse::ButtonDown(iw::MMOUSE))
	{
		if (iw::Keyboard::KeyDown(iw::SHIFT))
		{
			Bus->push<SpawnItem_Event>(sand->sP.x, sand->sP.y, 1, SpawnItem_Event::LASER_CHARGE);
		}

		else {
			Bus->push<SpawnItem_Event>(sand->sP.x, sand->sP.y, 1, SpawnItem_Event::HEALTH);
		}
	}

	if (iw::Keyboard::KeyDown(iw::X))
	{
		for (int i = 0; i < 5; i++) {
			for (int y = 0; y < 800; y++)
			{
				sand->m_world->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::ROCK));
			}
			x++;
		}
	}
}
