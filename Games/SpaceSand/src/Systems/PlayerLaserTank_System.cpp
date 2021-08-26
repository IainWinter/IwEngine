#include "Systems/PlayerLaserTank_System.h"

void PlayerLaserTankSystem::OnPush()
{
	m_fluidVelocity = glm::vec3(0.f);
	m_fluidToCreate = 0;
	m_fluidToRemove = 0;
	m_fluidCount = 0;

	m_fluidTime = 0.f;

	{ // laser fluid box
		iw::SandChunk* chunk = m_tankSand->m_world->GetChunk(0, 0); // only 1 chunk in this
		auto [w, h] = m_tankSand->GetSandTexSize();
			
		std::vector<int> widths = {
			4, 4,
			6, 
			8, 
			14, 
			24, 
			30, 
			34, 
			38, 38, 38,
			40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
			38, 38, 38,
			36, 36,
			34,
			30,
			24
		};

		std::vector<std::pair<int, int>> fills = { // these are slightly wrong
			{32, 36},
			{32, 37},

			{33, 33},
			{33, 34},
			{33, 35},
			{33, 36},
				
			{34, 32},
			{34, 33},
			{34, 34},
			{34, 35},
			{34, 36},
				
			{35, 31},
			{35, 32},
			{35, 33},
			{35, 34},
			{35, 35},
			{35, 36},
			{35, 37},
				
			{36, 33},
			{36, 34},
			{36, 35},
			{36, 36},
			{36, 37},

			{37, 33},
			{37, 34},
			{37, 35},
			{37, 36},
			{37, 37},
			{37, 38},

			{38, 34},
			{38, 35},
			{38, 36},
			{38, 37},
			{38, 38},
			{38, 39},

			{39, 35},
			{39, 36},
			{39, 37},
			{39, 38},

			{40, 35},
			{40, 36},
		};

		// for some reason this broke, not sure why
		// doesnt seem to collide against the solid field, but regular cells work??

		for (int y = 0; y < h; y++)
		for (int x = 0; x < w; x++)
		{
			//chunk->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::ROCK));
			chunk->SetCell(x, y, true, iw::SandField::SOLID);
		}

		for (int y = 0; y < widths.size(); y++)
		{
			int yw = widths[y] / 2;

			for (int x = w/2 - yw; x < w/2 + yw; x++)
			{
				//chunk->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::EMPTY));
				chunk->SetCell(x, y, false, iw::SandField::SOLID);
			}
		}

		for (const auto& [x, y] : fills)
		{
			//chunk->SetCell(x-2, y-2, iw::Cell::GetDefault(iw::CellType::ROCK));
			chunk->SetCell(x-2, y-2, true, iw::SandField::SOLID);
		}
	}
}

void PlayerLaserTankSystem::Update()
{
	iw::SandChunk* chunk = m_tankSand->m_world->GetChunk(0, 0);

	m_fluidVelocity = iw::lerp(m_fluidVelocity, m_player.Find<iw::Rigidbody>()->Velocity, iw::DeltaTime() * 5);

	std::pair<int, int> xy;

	for (int y = 0; y < chunk->m_height; y++)
	for (int x = 0; x < chunk->m_width;  x++)
	{
		chunk->KeepAlive(x, y);
		chunk->GetCell(x, y).dx = -m_fluidVelocity.x / 5; // this should match 'acceleration' even tho its instant
		//chunk->GetCell(x, y).dy = -m_fluidVelocity.y / 10;
	}

	if (m_fluidToCreate > 0)
	{
		iw::Cell laserFluid = iw::Cell::GetDefault(iw::CellType::WATER);
		laserFluid.Color = iw::Color::From255(255, 38, 38);

		int x = 29;
		int y = 31;

		if (chunk->IsEmpty(x, y))
		{
			chunk->SetCell(x, y, laserFluid);
			m_fluidToCreate--;
			m_fluidCount++;
		}
	}

	Player* player = m_player.Find<Player>(); // this is bad to need should be event based
	
	if (player->i_fire2)
	{
		for (int x = 18; x < 22; x++)
		{
			int y = 0;

			if (!chunk->IsEmpty(x, y))
			{
				chunk->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::EMPTY));
				m_fluidToRemove--;
				m_fluidCount--;

				m_fluidTime += m_perCellTime;
			}
		}
	}

	if (m_fluidCount == 0)
	{
		m_fluidToRemove = 0;
	}

	m_fluidTime = iw::max(m_fluidTime - iw::DeltaTime(), 0.f);

	player->can_fire_laser = m_fluidTime > 0.f;
}

bool PlayerLaserTankSystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case CREATED_PLAYER:
		{
			m_player = e.as<CreatedPlayer_Event>().PlayerEntity;
			break;
		}
		case CHANGE_LASER_FLUID:
		{
			ChangeLaserFluid_Event& event = e.as<ChangeLaserFluid_Event>();
			if (event.Amount > 0) m_fluidToCreate +=  event.Amount;
			if (event.Amount < 0) m_fluidToRemove += -event.Amount;
				
			break;
		}

		case PROJ_HIT_TILE:
		{
			ProjHitTile_Event& event = e.as<ProjHitTile_Event>();
			if (event.Hit.Has<Player>())
			{
				m_fluidVelocity.y -= 200;
				m_fluidVelocity.x -= 75 * iw::randfs();
			}

			break;
		}
	}

	return false;
}
