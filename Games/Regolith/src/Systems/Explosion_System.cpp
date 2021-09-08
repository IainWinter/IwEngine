#include "Systems/Explosion_System.h"

bool ExplosionSystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case SPAWN_EXPLOSION:
		{
			SpawnExplosion(e.as<SpawnExplosion_Event>().Config);
			break;
		}
	}

	return false;
}

void ExplosionSystem::SpawnExplosion(SpawnExplosion_Config& config)
{
	iw::Cell metal;
	metal.Type = iw::CellType::ROCK;
	metal.Props = iw::CellProp::MOVE_FORCE;
	metal.Color = iw::Color::From255(255, 207, 77);

	for (int i = 0; i < 200; i++)
	{
		glm::vec2 norm = glm::normalize(glm::vec2(iw::randfs(), iw::randfs()));
				
		metal.dx = norm.x * 300 * (iw::randf() * .9f + .1f);
		metal.dy = norm.y * 300 * (iw::randf() * .9f + .1f);
		metal.life = .15 + iw::randf() * .2;
		
		float margin = 15;
		float px = config.SpawnLocationX + norm.x * margin * (iw::randf() * .9f + .1f);
		float py = config.SpawnLocationY + norm.y * margin * (iw::randf() * .9f + .1f);

		if (iw::randf() > .75f)	{
			ShotInfo config;
			config.projectile = ProjectileType::BULLET;
			config.life = metal.life;
			config.dx = metal.dx;
			config.dy = metal.dy;
			config.x = px;
			config.y = py;
			Bus->push<SpawnProjectile_Event>(config);
		}
		else {
			sand->m_world->SetCell(px, py, metal);
		}
	}

	iw::Cell smoke = iw::Cell::GetDefault(iw::CellType::SMOKE);

	for (int y = -5; y < 5; y++)
	for (int x = -5; x < 5; x++)
	{
		int px = x + config.SpawnLocationX;
		int py = y + config.SpawnLocationY;
		smoke.life = iw::randf() * 5;

		sand->m_world->SetCell(px, py, smoke);
	}
}
