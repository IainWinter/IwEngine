#include "Systems/Projectile_System.h"

void ProjectileSystem::Update()
{
	Space->Query<iw::Transform, iw::Rigidbody, ProjHead>().Each([&](
		iw::EntityHandle handle, 
		iw::Transform* transform,
		iw::Rigidbody* rigidbody,
		ProjHead* projhead)
	{
		Projectile* projectile = projhead->Proj;
		
		if (projectile->Life > 0)
		{
			projectile->Life -= iw::DeltaTime();
			if (projectile->Life <= 0.f)
			{
				Space->QueueEntity(handle, iw::func_Destroy);
			}
		}

		iw::Entity entity = iw::Entity(handle, Space);

		if (projectile->OnUpdate(entity))
		{ 
			Space->QueueEntity(handle, iw::func_Destroy);
		}

		glm::vec3& pos = transform->Position;
		glm::vec3& vel = rigidbody->Velocity;

		int zIndex = ceil(pos.z);

		HitInfo hitInfo;

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
				hitInfo.hit = true; // collision on no chunk
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
							config.Projectile = entity;
							config.Hit = Space->FindEntity<iw::Tile>(info.tile);

							Bus->push<RemoveCellFromTile_Event>(info.index, config.Hit);
							Bus->push<ProjHitTile_Event>(config);
						}
						
						// stop multiple events for same index
						info.tile = nullptr;
						info.index = 0;

						hitInfo.hit = true;
					}
				}

				else
				if (  !chunk->IsEmpty(px, py)
					&& chunk->GetCell(px, py).Type != iw::CellType::PROJECTILE)
				{
					hitInfo.hit = true;
				}

				projectile->DrawCell(sand, chunk, px, py, vel.x, vel.y);
			}

			if (hitInfo.hit)
			{
				hitInfo.x = fpx;
				hitInfo.y = fpy;
				return true;
			}

			return false;
		});

		if (hitInfo.hit)
		{
			hitInfo.sand = sand;
			hitInfo.bus = Bus;
			hitInfo.entity = entity;

			projectile->OnHit(hitInfo);
		}
	});
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
				case RUN_STATE:
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

iw::Entity ProjectileSystem::MakeProjectile(
	const ShotInfo& shot)
{
	iw::Entity entity = Space->CreateEntity<
		iw::Transform, 
		iw::Rigidbody,
		iw::Circle,
		ProjHead>();

	iw::Transform* transform = entity.Set<iw::Transform>();
	iw::Rigidbody* rigidbody = entity.Set<iw::Rigidbody>();
	iw::Circle*    collider  = entity.Set<iw::Circle>();

	transform->Position = glm::vec3(shot.x, shot.y, 0);

	rigidbody->SetTransform(transform);
	rigidbody->Collider = collider;
	rigidbody->Velocity = glm::vec3(shot.dx, shot.dy, 0);
	rigidbody->IsTrigger = true;

	Physics->AddRigidbody(rigidbody);

	return entity;
}

iw::Entity ProjectileSystem::MakeBullet(
	const ShotInfo& shot,
	int depth)
{
	iw::Entity entity = MakeProjectile(shot);
	entity.Set<ProjHead>(MakeBullet_Projectile(shot, depth));

	return entity;
}

iw::Entity ProjectileSystem::MakeLaser(
	const ShotInfo& shot,
	int depth)
{
	iw::Entity entity = MakeProjectile(shot);
	entity.Set<ProjHead>(MakeLaser_Projectile(shot, depth));

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
