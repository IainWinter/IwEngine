#include "Systems/World_System.h"

int WorldSystem::Initialize()
{
	auto [w, h] = sand->GetSandTexSize();
	sand->SetCamera(w / 2, h / 2);

	Spawn spawn(5, 1.f, .2f, [=](float x, float y)
	{
		auto [w, h] = sand->GetSandTexSize();
		float margin = .1f;

		float target_x = iw::randi(w - w * margin * 2) + w * margin;
		float target_y = iw::randi(h - h * margin * 2) + h * margin;

		Bus->push<SpawnEnemy_Event>(m_player, x, y, target_x, target_y);
	});
	spawn.AddSpawn(0, h + 10, w, h + 20);
	spawn.AddSpawn(0,   - 10, w,   - 20);

	Fill fill([=](int x, int y) 
	{
		sand->m_world->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::ROCK));
	});
	fill.AddFill(0, h - 100, w, h - 120, Fill::LEFT_RIGHT);
	fill.AddFill(0,     100, w,     120, Fill::LEFT_RIGHT);
	fill.AddFill(w - 100, 0, w - 120, h);
	fill.AddFill(    100, 0,     120, h);

	iw::EventSequence& level1 = m_levels.emplace_back(CreateSequence());

	level1.Add<Spawn>(spawn);
	level1.Add<Fill>(fill);
	level1.Add([&]() {
		m_levels.pop_back();
		return true;
	});

	m_levels.back().Start();

	return 0;
}

void WorldSystem::Update()
{
	if (m_levels.size() > 0)
	{
		m_levels.back().Update();
	}
}

bool once = true;
int x = 0;

void WorldSystem::FixedUpdate()
{
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

	// DEBUG

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
