#include "Systems/Projectile_System.h"

// add the tile velocity to the projectile velocity

void ProjectileSystem::Update()
{
	// dps is based on framerate ...

	for (auto [entity, transform, projectile] : entities().query<iw::Transform, Projectile>().with_entity())
	{
		if (glm::distance2(transform.Position, glm::vec3(200.f, 200.f, 0.f)) > 400 * 400)
		{
			entities_defer().destroy(entity);
		}

		else
		if (projectile.Life > 0)
		{
			projectile.Life -= iw::DeltaTime();
			if (projectile.Life <= 0.f)
			{
				entities_defer().destroy(entity);
			}
		}
	}

	for (auto components : entities().query<iw::Transform, iw::Rigidbody, Projectile, Linear_Projectile>().with_entity())
	{
		entity             entity    = std::get<0>(components);
		iw::Transform&     transform = std::get<1>(components);
		iw::Rigidbody&     rigidbody = std::get<2>(components);
		Projectile&        proj      = std::get<3>(components);
		Linear_Projectile& linear    = std::get<4>(components);

		glm::vec3& pos = transform.Position;
		glm::vec3& vel = rigidbody.Velocity;
		int zIndex = ceil(pos.z);
		
		// idea:
		//	issue is that the projectiles move in the physics time step, but that doesnt check for cells in the way
		// solves:
		//	removed physics object from movement sim
		//	now this is more frame dependent. Maybe this is when they should split, hit detection in fixed step and draw here
		
		sand->ForEachInLine(
			pos.x, pos.y, 
			pos.x + vel.x * iw::DeltaTime(), pos.y + vel.y * iw::DeltaTime(),
		[&](
			float fpx, float fpy)
		{
			int px = floor(fpx);
			int py = floor(fpy);
		
			iw::SandChunk* chunk = sand->m_world->GetChunk(px, py);
		
			if (chunk)
			{
				iw::TileInfo& info = chunk->GetCell<iw::TileInfo>(px, py, iw::SandField::TILE_INFO);
				if (info.tile)
				{
					if (info.tile->m_zIndex == zIndex)
					{
						if (info.tile->m_initalCellCount != 0) // temp hack to fix left behind pixels from tiles, todo: find cause of left behind pixels
						{
							ProjHitTile_Config config;
							config.X = px;
							config.Y = py;
							config.Info = info;
							config.Projectile = entity;

							auto [exists, entity] = entities().find(info.tile);

							if (exists)
							{
								config.Hit = entity;

								// exit if no damage on player
								if (   config.Hit.has<Player>()
									&& config.Hit.get<Player>().NoDamage)
								{
									return false;
								}
							}
		
							Bus->push<RemoveCellFromTile_Event>(info.index, config.Hit);
							Bus->push<ProjHitTile_Event>(config);
						}
					
						// stop multiple events for same index
						info.tile = nullptr;
						info.index = 0;
		
						proj.Hit.HasContact = true;
					}
				}
		
				else
				if (  !chunk->IsEmpty(px, py)
					&& chunk->GetCell(px, py).Type != iw::CellType::PROJECTILE)
				{
					proj.Hit.HasContact = true;
				}
		
				//if (!proj->Hit.HasContact)
				//{
					// overwrites the cell, maybe we want this to only happen if no contact?
					chunk->SetCell(px, py, linear.Cell);
				//}
			}
		
			if (proj.Hit.HasContact)
			{
				proj.Hit.x = fpx;
				proj.Hit.y = fpy;
				
				return true;
			}
		
			return false;
		});
		
		if (proj.Hit.HasContact)
		{
			pos.x = proj.Hit.x;
			pos.y = proj.Hit.y;
		}
		
		else
		{
			pos.x = pos.x + vel.x * iw::DeltaTime();
			pos.y = pos.y + vel.y * iw::DeltaTime();
		}
	}

	for (auto [entity, transform, rigidbody, proj, split] : entities().query<iw::Transform, iw::Rigidbody, Projectile, Split_Projectile>().with_entity())
	{
		if (!proj.Hit.HasContact)
		{
			continue;
		}

		int hx = floor(proj.Hit.x);
		int hy = floor(proj.Hit.y);

		if (   split.Split < split.MaxSplit
			&& sand->m_world->InBounds(hx, hy))
		{
			glm::vec3& pos = transform.Position;
			glm::vec3& vel = rigidbody.Velocity;

			for (int i = 0; i < split.ShrapCount + 1; i++)
			{
				if (i > 0 && iw::randf() > split.ShrapOdds)
				{
					continue;
				}

				float speed = glm::length(vel);
				auto [dx, dy] = randvel(rigidbody, i == 0 ? split.SplitTurnArc : split.ShrapTurnArc);

				ShotInfo shot = proj.Shot;
				shot.x = proj.Hit.x;
				shot.y = proj.Hit.y;
				shot.dx = dx;
				shot.dy = dy;

				// the time scale effects this greatly,
				// I think there is an event loop I think a physics update
				// happens in the middle causing issues?

				//LOG_INFO << shot.x << ", " << shot.y;

				int depth = split.Split + 1;
				Bus->push<SpawnProjectile_Event>(shot, depth);
			}
		}

		entities_defer().destroy(entity);
	}

	// Projectiles that are made out of a sprite

	//Space->Query<iw::Transform, iw::Rigidbody, Projectile, Tile_Projectile>().Each(
	//	[&](
	//		entityHandle handle,
	//		iw::Transform*   transform,
	//		iw::Rigidbody*   rigidbody,
	//		Projectile*      projectile,
	//		Tile_Projectile* tileP)
	//	{
	//		
	//	}
	//);

	// Projectiles that draw lightning

	std::vector<LightningConfig> bolts;

	for (auto [entity, transform, rigidbody, proj, ball] : entities().query<iw::Transform, iw::Rigidbody, Projectile, LightBall_Projectile>().with_entity())
	{
		glm::vec3& pos = transform.Position;
		
		//auto& [angle, dangle] = iw::choose_e(ball.Points);
		for (auto& [angle, dangle] : ball.Points)
		{
			angle += .01 * dangle;

			float length = 15;
			float dx = cos(angle);
			float dy = sin(angle);

			iw::CollisionObject* object = Physics->QueryVector(
				pos, glm::vec3(dx, dy, 0.f), 75, 35).Closest();

			LightningConfig config;
			config.A = entity;
			config.Owner = proj.Shot.origin;
			config.ArcSize = 5;
			config.LifeTime = iw::DeltaTime();
			config.StopOnHit = true;

			if (   object
				&& object != &rigidbody
				&& proj.Shot.origin != GetPhysicsEntity(object).second)
			{
				config.Type = LightningType::ENTITY;
				config.B = GetPhysicsEntity(object).second;
			}

			else
			{
				config.Type = LightningType::POINT;
				config.X = (int)floor(pos.x);
				config.Y = (int)floor(pos.y);
				config.TargetX = (int)floor(pos.x + dx * length);
				config.TargetY = (int)floor(pos.y + dy * length);
			}

			bolts.push_back(config);
		}
	}

	for (auto [entity, proj, bolt] : entities().query<Projectile, LightBolt_Projectile>().with_entity())
	{
		const ShotInfo& shot = proj.Shot;
		
		float tx = shot.x + shot.dx;
		float ty = shot.y + shot.dy;
		
		iw::DistanceQueryResult query = Physics->QueryVector(
			glm::vec3(shot.x,  shot.y,  .0f), 
			glm::vec3(shot.dx, shot.dy, .0f), shot.Speed(), 64);
		
		LightningConfig config;
		config.A = entity;
		config.Owner = proj.Shot.origin;
		config.LifeTime = .01f + iw::randf() * .05f;
		config.StopOnHit = true;
		config.ArcSize = 10 + iw::randi(6) - 3;

		if (query.Objects.size() > 0)
		{
			auto [dist, obj] = iw::choose_e(query.Objects);
		
			config.Type = LightningType::ENTITY;
			config.A = shot.origin;
			config.B = GetPhysicsEntity(obj).second;
		
			bolts.push_back(config);
		}
		
		else
		{
			auto cells = FindClosestCellPositionsMatchingTile(
				sand, shot.origin.get<iw::Tile>(),
				(int)floor(shot.x), (int)floor(shot.y));
		
			if (cells.size() > 0)
			{
				CellInfo info = iw::choose_e(cells);
			
				config.X = info.x;
				config.Y = info.y;
				config.TargetX = tx + iw::randi(20) - 10;
				config.TargetY = ty + iw::randi(20) - 10;
		
				bolts.push_back(config);
			}
		}

		entities_defer().destroy(entity);
	}

	for (LightningConfig& bolt : bolts)
	{
		LightningHitInfo hit;

		if (bolt.Type == LightningType::ENTITY)
		{
			hit = DrawLightning(sand, Space, bolt);
		}

		else {
			hit = DrawLightning(sand, bolt);
		}
		
		if (hit.HasContact)
		{
			if (bolt.B.is_alive())
			{
				if (!::GetPhysicsComponent(bolt.B)->Collider)
				{
					entities_defer().destroy(bolt.B);
				}
			}

			if (hit.TileInfo.tile)
			{
				entity entity = entities().find(hit.TileInfo.tile).second;

				if (entity == bolt.Owner) continue;

				if (   entity.has<Player>()
					|| entity.has<Enemy>()
					|| entity.has<Asteroid>())
				{
					ProjHitTile_Config config;
					config.X = hit.X;
					config.Y = hit.Y;
					config.Info = hit.TileInfo;
					config.Projectile = bolt.A;
					config.Hit = entity;

					Bus->push<RemoveCellFromTile_Event>(hit.TileInfo.index, config.Hit);
					Bus->push<ProjHitTile_Event>(config);
				}
			}

			sand->m_world->SetCell(hit.X, hit.Y, iw::Cell());
		}
	}
}

bool ProjectileSystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case SPAWN_PROJECTILE: {
			SpawnProjectile_Event& event = e.as<SpawnProjectile_Event>();

			std::vector<ShotInfo> shots = event.Shot.others;
			shots.push_back(event.Shot);

			for (const ShotInfo& shot : shots)
			{
				switch (event.Shot.projectile) 
				{	
					case ProjectileType::BULLET: MakeBullet(event.Shot, event.Depth); break;
					case ProjectileType::LASER:  MakeLaser (event.Shot, event.Depth); break;
					case ProjectileType::WATTZ:  MakeWattz (event.Shot, event.Depth); break;
					case ProjectileType::BOLTZ:  MakeBoltz (event.Shot, event.Depth); break;
					//case ProjectileType::BEAM:   MakeBeam  (event.Shot, event.Depth); break;
				}
			}	
			break;
		}
	}

	return false;
}

entity ProjectileSystem::MakeBullet(
	const ShotInfo& shot,
	int depth)
{
	entity entity = MakeProjectile<Linear_Projectile, Split_Projectile>(shot);
	Linear_Projectile& linear = entity.get<Linear_Projectile>();
	Split_Projectile&  split  = entity.get<Split_Projectile>();
	
	linear.Cell.life = .02;
	linear.Cell.Color = iw::Color::From255(255, 230, 66);
	split.SplitTurnArc = iw::Pi / 4;
	split.MaxSplit = 15;
	split.Split = depth;

	return entity;
}

entity ProjectileSystem::MakeLaser(
	const ShotInfo& shot,
	int depth)
{
	entity entity = MakeProjectile<Linear_Projectile, Split_Projectile>(shot);
	Linear_Projectile& linear = entity.get<Linear_Projectile>();
	Split_Projectile&  split  = entity.get<Split_Projectile>();

	linear.Cell.life = .2f;
	linear.Cell.Color = iw::Color::From255(255, 23, 6);
	split.MaxSplit = 25;
	split.ShrapCount = 2;
	split.ShrapOdds = .05f;
	split.ShrapTurnArc = iw::Pi / 6;
	split.SplitTurnArc = iw::Pi / 18;
	split.Split = depth;

	return entity;
}

entity ProjectileSystem::MakeWattz(
	const ShotInfo& shot,
	int depth)
{
	entity entity = MakeProjectile<LightBall_Projectile>(shot);

	entity.get<iw::Rigidbody>().IsKinematic = true;

	LightBall_Projectile& ball = entity.get<LightBall_Projectile>();

	float da = iw::randfs() * iw::Pi;
	for (int i = 0; i < 10; i++)
	{
		ball.Points.emplace_back(iw::Pi2 / 10.f * i, da);
	}

	return entity;
}

entity ProjectileSystem::MakeBoltz(
	const ShotInfo& shot,
	int depth)
{
	entity entity = MakeProjectile_raw<LightBolt_Projectile>(shot);
	return entity;
}

//entity ProjectileSystem::MakeBeam(const ShotInfo& shot, int depth)
//{
//	entity entity = MakeProjectile(shot);
//	Projectile* projectile = entity.Find<Projectile>();
//
//	projectile.PlaceCell = [=](iw::SandChunk* chunk, int px, int py, float dx, float dy)
//	{
//		iw::Color centerColor = iw::Color::From255(252, 239, 91);
//		iw::Color outerColor  = iw::Color::From255(255, 66, 255);
//
//		iw::Cell cell;
//		cell.Type = iw::CellType::PROJECTILE;
//		cell.Props = iw::CellProp::MOVE_FORCE;
//
//		for (int x = -3; x < 3; x++)
//		for (int y = -3; y < 3; y++)
//		{
//			float dist = sqrt(x*x + y*y) / sqrt(3*3+3*3);
//
//			cell.Color = iw::lerp(centerColor.rgb(), outerColor.rgb(), dist);
//			cell.dx = iw::randfs() * 10 * (dist * 2 + 1);
//			cell.dy = iw::randfs() * 10 * (dist * 2 + 1);
//			cell.life = iw::randf()*2;
//
//			sand->m_world->SetCell(px, py, cell);
//		}
//	};
//
//	projectile->OnHit = [=](float fhx, float fhy)
//	{
//		int hx = floor(fhx);
//		int hy = floor(fhy);
//
//		if (depth < 25 && sand->m_world->InBounds(hx, hy))
//		{
//			auto [dx2, dy2] = randvel(entity, iw::Pi/80);
//			
//			ShotInfo split = shot;
//			split.x = fhx; // should make faster could fill line
//			split.y = fhy;
//			split.dx = dx2;
//			split.dy = dy2;
//			
//			Bus->push<SpawnProjectile_Event>(split, depth); // bores forever
//		}
//
//		Space->QueueEntity(entity.Handle, iw::func_Destroy);
//	};
//
//	return entity;
//}

//void ProjectileSystem::MakeExplosion(
//	int x, int y,
//	int r)
//{
//	for (int px = x - r; px < x + r; px++)
//	for (int py = y - r; py < y + r; py++)
//	{
//		if (iw::SandChunk* chunk = sand->m_world->GetChunk(px, py))
//		{
//			auto[tile, index] = chunk->GetCell<iw::TileInfo>(px, py, iw::SandField::TILE_INFO);
//
//			if (tile)
//			{
//				sand->EjectPixel(tile, index);
//			}
//
//			chunk->SetCell(px, py, iw::Cell::GetDefault(iw::CellType::EMPTY));
//		}
//	}
//}
