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
	iw::Cell spark;
	spark.Type = iw::CellType::ROCK;
	spark.Props = iw::CellProp::MOVE_FORCE;
	spark.Color = iw::Color::From255(255, 207, 77);

	for (int i = 0; i < config.ExplosionPower/* * config.ExplosionRadius*/; i++)
	{
		glm::vec2 norm = glm::normalize(glm::vec2(iw::randfs(), iw::randfs()));
				
		spark.dx = norm.x * 300 * (iw::randf() * .9f + .1f);
		spark.dy = norm.y * 300 * (iw::randf() * .9f + .1f);
		spark.life = .15 + iw::randf() * .2;
		
		float px = config.SpawnLocationX + norm.x * config.ExplosionRadius * (iw::randf() * .9f + .1f);
		float py = config.SpawnLocationY + norm.y * config.ExplosionRadius * (iw::randf() * .9f + .1f);

		//if (iw::randf() > .75f)	{
			ShotInfo shot;
			shot.projectile = ProjectileType::BULLET;
			shot.life = spark.life;
			shot.dx = 300;// spark.dx * 1.2f;
			shot.dy = 300;// spark.dy * 1.2f;
			shot.x = 200;// px;
			shot.y = 200;// py;
			Bus->push<SpawnProjectile_Event>(shot);
		//}
		//else {
		//	sand->m_world->SetCell(px, py, spark);
		//}
	}

	//iw::Cell smoke = iw::Cell::GetDefault(iw::CellType::SMOKE);

	//for (int y = -5; y < 5; y++)
	//for (int x = -5; x < 5; x++)
	//{
	//	int px = x + config.SpawnLocationX;
	//	int py = y + config.SpawnLocationY;
	//	smoke.life = iw::randf() * 5;

	//	sand->m_world->SetCell(px, py, smoke);
	//}
}
