#include "Systems/Projectile_System.h"

void ProjectileSystem::Update()
{
	Space->Query<iw::Transform, Projectile>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Transform* transform,
			Projectile* proj)
		{
			if (proj->Life > 0)
			{
				proj->Life -= iw::DeltaTime();
				if (proj->Life <= 0.f)
				{
					Space->QueueEntity(handle, iw::func_Destroy);
				}
			}

			if (glm::distance2(transform->Position, glm::vec3(200.f, 200.f, 0.f)) > 400 * 400)
			{
				Space->QueueEntity(handle, iw::func_Destroy);
			}
		});


	Space->Query<iw::Transform, iw::Rigidbody, Projectile, Linear_Projectile>().Each(
		[&](
			iw::EntityHandle handle, 
			iw::Transform* transform,
			iw::Rigidbody* rigidbody,
			Projectile* proj,
			Linear_Projectile* linear)
		{
			glm::vec3& pos = transform->Position;
			glm::vec3& vel = rigidbody->Velocity;
			int zIndex = ceil(pos.z);

			sand->ForEachInLine(
				pos.x, pos.y, 
				pos.x + vel.x * iw::DeltaTime(), pos.y + vel.y * iw::DeltaTime(), 
			[&](
				float fpx, float fpy)
			{
				int px = floor(fpx);
				int py = floor(fpy);

				iw::SandChunk* chunk = sand->m_world->GetChunk(px, py);
			
				if (!chunk)
				{
					proj->Hit.HasContact = true; // collision on no chunk
				}

				else {
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
								config.Projectile = Space->GetEntity(handle);
								config.Hit = Space->FindEntity<iw::Tile>(info.tile);

								Bus->push<RemoveCellFromTile_Event>(info.index, config.Hit);
								Bus->push<ProjHitTile_Event>(config);
							}
						
							// stop multiple events for same index
							info.tile = nullptr;
							info.index = 0;

							proj->Hit.HasContact = true;
						}
					}

					else
					if (  !chunk->IsEmpty(px, py)
						&& chunk->GetCell(px, py).Type != iw::CellType::PROJECTILE)
					{
						proj->Hit.HasContact = true;
					}

					chunk->SetCell(px, py, linear->Cell);
				}

				if (proj->Hit.HasContact)
				{
					proj->Hit.x = fpx;
					proj->Hit.y = fpy;
					return true;
				}

				return false;
			});
		});

	Space->Query<iw::Transform, iw::Rigidbody, Projectile, Split_Projectile>().Each(
		[&](
			iw::EntityHandle  handle,
			iw::Transform* transform,
			iw::Rigidbody* rigidbody,
			Projectile*       proj, 
			Split_Projectile* split)
		{
			if (!proj->Hit.HasContact)
			{
				return;
			}

			int hx = floor(proj->Hit.x);
			int hy = floor(proj->Hit.y);

			if (   split->Split < split->MaxSplit
				&& sand->m_world->InBounds(hx, hy))
			{
				glm::vec3& pos = transform->Position;
				glm::vec3& vel = rigidbody->Velocity;

				for (int i = 0; i < split->ShrapCount + 1; i++)
				{
					if (i > 0 && iw::randf() > split->ShrapOdds)
					{
						continue;
					}

					float speed = glm::length(vel);
					auto [dx2, dy2] = randvel(rigidbody, i == 0 ? split->SplitTurnArc : split->ShrapTurnArc);

					ShotInfo shot = proj->Shot;
					shot.x = proj->Hit.x;
					shot.y = proj->Hit.y;
					shot.dx = dx2;
					shot.dy = dy2;

					Bus->push<SpawnProjectile_Event>(shot, split->Split + 1);
				}
			}

			Space->QueueEntity(handle, iw::func_Destroy);
		});

	// Projectiles that draw lightning

	std::vector<LightningConfig> bolts;

	Space->Query<iw::Transform, iw::Rigidbody, Projectile, LightBall_Projectile>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Transform* transform,
			iw::Rigidbody* rigidbody,
			Projectile* proj,
			LightBall_Projectile* ball)
		{	
			ball->Timer.Tick();
			if (ball->Timer.Can("draw"))
			{
				glm::vec3& pos = transform->Position;

				for (auto& [angle, dangle] : ball->Points)
				{
					angle += .01 * dangle;

					float al = 15;
					float ax = cos(angle) * al;
					float ay = sin(angle) * al;

					LightningConfig config;
					config.Type = LightningType::POINT;
					config.X       = (int)floor(pos.x);
					config.Y       = (int)floor(pos.y);
					config.TargetX = (int)floor(pos.x + ax);
					config.TargetX = (int)floor(pos.y + ay);
					config.ArcSize = 5;
					config.LifeTime = .01f;

					bolts.push_back(config);
				}
			}
		});

	Space->Query<iw::Transform, iw::Rigidbody, Projectile, LightBolt_Projectile>().Each(
		[&](
			iw::EntityHandle handle,
			iw::Transform* transform,
			iw::Rigidbody* rigidbody,
			Projectile* proj,
			LightBolt_Projectile* bolt)
		{	
			ShotInfo& shot = bolt->Shot;

			float tx = shot.x + shot.dx;
			float ty = shot.y + shot.dy;

			iw::CollisionObject* object = Physics->QueryDistance(
				glm::vec3(tx, ty, .0f), 100).Closest();

			LightningConfig config;
			config.ArcSize = 10;
			config.LifeTime = .0001f;
			config.StopOnHit = false;

			if (object)
			{
				config.Type = LightningType::ENTITY;
				config.A = shot.origin;
				config.B = iw::GetPhysicsEntity(Space, object);
			}

			else
			{
				config.X       = (int)floor(shot.x);
				config.Y       = (int)floor(shot.y);
				config.TargetX = (int)floor(shot.x + shot.dx);
				config.TargetY = (int)floor(shot.y + shot.dy);
			}

			bolts.push_back(config);

			Space->QueueEntity(handle, iw::func_Destroy);
		});


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
			if (hit.TileInfo.tile)
			{
				sand->EjectPixel(hit.TileInfo.tile, hit.TileInfo.index);
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
		case STATE_CHANGE:
		{
			StateChange_Event& event = e.as<StateChange_Event>();

			switch (event.State)
			{
				case GAME_START_STATE:
					Space->Query<ProjHead>().Each([&](iw::EntityHandle handle, ProjHead*) {
						Space->QueueEntity(handle, iw::func_Destroy);
					});
					break;
			}
			break;
		}
	}

	return false;
}

iw::Entity ProjectileSystem::MakeBullet(
	const ShotInfo& shot,
	int depth)
{
	iw::Entity entity = MakeProjectile<Linear_Projectile, Split_Projectile>(shot);
	Linear_Projectile* linear = entity.Set<Linear_Projectile>();
	Split_Projectile*  split  = entity.Set<Split_Projectile>(depth);
	
	linear->Cell.life = .02;
	linear->Cell.Color = iw::Color::From255(255, 230, 66);
	split->SplitTurnArc = iw::Pi / 4;
	split->MaxSplit = 15;

	return entity;
}

iw::Entity ProjectileSystem::MakeLaser(
	const ShotInfo& shot,
	int depth)
{
	iw::Entity entity = MakeProjectile<Linear_Projectile, Split_Projectile>(shot);
	Linear_Projectile* linear = entity.Set<Linear_Projectile>();
	Split_Projectile*  split  = entity.Set<Split_Projectile>(depth);

	linear->Cell.life = .2f;
	linear->Cell.Color = iw::Color::From255(255, 23, 6);
	split->MaxSplit = 25;
	split->ShrapCount = 1;
	split->ShrapOdds = .1f;
	split->ShrapTurnArc = iw::Pi / 6;

	return entity;
}

iw::Entity ProjectileSystem::MakeWattz(
	const ShotInfo& shot,
	int depth)
{
	iw::Entity entity = MakeProjectile<LightBall_Projectile>(shot);
	LightBall_Projectile* ball = entity.Set<LightBall_Projectile>();

	for (int i = 0; i < 10; i++)
	{
		ball->Points.emplace_back(
			iw::randfs() * iw::Pi2, 
			iw::randfs() * iw::Pi);
	}

	ball->Timer.SetTime("draw", 0.01f);

	return entity;
}

iw::Entity ProjectileSystem::MakeBoltz(
	const ShotInfo& shot,
	int depth)
{
	iw::Entity entity = MakeProjectile<LightBolt_Projectile>(shot);
	LightBolt_Projectile* bolt = entity.Set<LightBolt_Projectile>();

	bolt->Shot = shot;

	return entity;
}

//iw::Entity ProjectileSystem::MakeBeam(const ShotInfo& shot, int depth)
//{
//	iw::Entity entity = MakeProjectile(shot);
//	Projectile* projectile = entity.Find<Projectile>();
//
//	projectile->PlaceCell = [=](iw::SandChunk* chunk, int px, int py, float dx, float dy)
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
