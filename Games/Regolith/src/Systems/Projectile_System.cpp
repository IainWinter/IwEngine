#include "Systems/Projectile_System.h"

void ProjectileSystem::Update()
{
	Space->Query<Projectile>().Each([&](iw::EntityHandle, Projectile* p) 
	{
		if (p->Update)
		{
			auto [hit, hx, hy] = p->Update();

			if (p->OnHit && hit) 
			{
				p->OnHit(hx, hy);
			}
		}
	});
}

//float time__ = 0;

void ProjectileSystem::FixedUpdate()
{
	Space->Query<Projectile>().Each([&](iw::EntityHandle, Projectile* p) 
	{
		if (p->FixedUpdate) p->FixedUpdate();
	});

	//time__ += iw::FixedTime();

	//if (/*iw::Keyboard::KeyDown(iw::G) &&*/ time__ > .15)
	//{
	//	time__ = 0;

	//	float x = 0;// iw::Mouse::ScreenPos().x() / Renderer->Width();
	//	float y = 1;// iw::Mouse::ScreenPos().y() / Renderer->Height();

	//	auto [w, h] = sand->GetSandTexSize2();

	//	glm::vec2 vel(x, y);
	//	vel = glm::normalize(vel) * 44.f;
	//	//vel.x += iw::randfs() * 5;
	//	//vel.y += iw::randfs() * 5;

	//	ShotInfo shot;
	//	shot.x = w-4;
	//	shot.y = h-20;
	//	shot.dx = vel.x;
	//	shot.dy = vel.y;
	//	shot.projectile = ProjectileType::BULLET;

	//	Bus->push<SpawnProjectile_Event>(shot, 15);
	//}
}

// helpers

auto getproj(iw::Entity entity)
{
	return entity.Find<Projectile>();
}
auto getpos(iw::Entity entity)
{
	iw::Rigidbody* r = entity.Find<iw::Rigidbody>();
	iw::Transform* t = entity.Find<iw::Transform>();

	float x = t->Position.x;
	float y = t->Position.y;
	int   z = floor(t->Position.z); // Z Index

	float dx = r->Velocity.x;
	float dy = r->Velocity.y;

	return std::tuple(x, y, z, dx, dy);
};
auto setpos(iw::Entity entity, float x, float y)
{
	iw::Rigidbody* r = entity.Find<iw::Rigidbody>();
	iw::Transform& t = r->Transform;

	t.Position.x = x;
	t.Position.y = y;
};
auto setvel(iw::Entity entity, float x, float y)
{
	iw::Rigidbody* r = entity.Find<iw::Rigidbody>();

	r->Velocity.x = x;
	r->Velocity.y = y;
};
auto randvel(iw::Entity entity, float amount, bool setForMe = false) 
{
	iw::Rigidbody* r = entity.Find<iw::Rigidbody>();
	float dx = r->Velocity.x;
	float dy = r->Velocity.y;

	float speed = sqrt(dx * dx + dy * dy);
	float angle = atan2(dy, dx);
	angle += iw::randfs() * amount;

	dx = cos(angle) * speed;
	dy = sin(angle) * speed;

	if (setForMe) {
		setvel(entity, dx, dy);
	}

	return std::tuple(dx, dy);
};

bool ProjectileSystem::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case SPAWN_PROJECTILE: {
			SpawnProjectile_Event& event = e.as<SpawnProjectile_Event>();

			switch (event.Shot.projectile) 
			{
				case ProjectileType::BULLET: MakeBullet(event.Shot, event.Depth); break;
				case ProjectileType::LASER:  MakeLaser (event.Shot, event.Depth); break;
				case ProjectileType::BEAM:   MakeBeam  (event.Shot, event.Depth); break;
			}
			break;
		}
		case STATE_CHANGE:
		{
			StateChange_Event& event = e.as<StateChange_Event>();

			switch (event.State)
			{
				case RUN_STATE:
					Space->Query<Projectile>().Each([&](iw::EntityHandle handle, Projectile*) {
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
		Projectile>();

	Projectile*    projectile = entity.Set<Projectile>();
	iw::Transform* transform  = entity.Set<iw::Transform>();
	iw::Circle*    collider   = entity.Set<iw::Circle>();
	iw::Rigidbody* rigidbody  = entity.Set<iw::Rigidbody>();

	transform->Position = glm::vec3(shot.x, shot.y, 0);

	rigidbody->SetTransform(transform);
	rigidbody->Velocity = glm::vec3(shot.dx, shot.dy, 0);
	rigidbody->Collider = collider;
	rigidbody->IsTrigger = true;

	Physics->AddRigidbody(rigidbody);

	projectile->Update = [=]() 
	{
		auto [x, y, zIndex_, dx_, dy_] = getpos(entity);
		int zIndex = zIndex_; // for unpacking nonsense
		float dx = dx_;
		float dy = dy_;

		bool hit = false;
		float hx, hy;

		sand->ForEachInLine(x, y, x + dx * iw::DeltaTime(), y + dy * iw::DeltaTime(), [&](
			float fpx, float fpy)
		{
			int px = floor(fpx);
			int py = floor(fpy);

			iw::SandChunk* chunk = sand->m_world->GetChunk(px, py);
			
			if (!chunk)
			{
				hit = true; // collision on no chunk
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

						hit = true;
					}
				}

				else
				if (  !chunk->IsEmpty(px, py)
					&& chunk->GetCell(px, py).Type != iw::CellType::PROJECTILE)
				{
					hit = true;
				}

				getproj(entity)->PlaceCell(chunk, px, py, dx, dy);
			}

			if (hit) 
			{
				hx = fpx;
				hy = fpy;
				return true;
			}

			return false;
		});

		return std::tuple(hit, hx, hy);
	};

	return entity;
}

iw::Entity ProjectileSystem::MakeBullet(
	const ShotInfo& shot,
	int depth)
{
	iw::Entity entity = MakeProjectile(shot);
	Projectile* projectile = entity.Find<Projectile>();

	projectile->PlaceCell = [=](iw::SandChunk* chunk, int px, int py, float dx, float dy)
	{
		iw::Cell cell;
		cell.Type = iw::CellType::PROJECTILE;
		cell.Color = iw::Color::From255(255, 230, 66);
		cell.life = .02;

		chunk->SetCell(px, py, cell);
	};

	projectile->OnHit = [=](float fhx, float fhy)
	{
		int hx = floor(fhx);
		int hy = floor(fhy);

		if (depth < 15 && sand->m_world->InBounds(hx, hy))
		{
			auto [x, y, z, dx, dy] = getpos(entity);
			float speed = sqrt(dx*dx + dy*dy);
			auto [dx2, dy2] = randvel(entity, iw::Pi/4);

			ShotInfo split = shot;
			split.x = fhx;
			split.y = fhy;
			split.dx = dx2;
			split.dy = dy2;

			Bus->push<SpawnProjectile_Event>(split, depth + 1);
		}

		Space->QueueEntity(entity.Handle, iw::func_Destroy);
	};

	return entity;
}

iw::Entity ProjectileSystem::MakeLaser(
	const ShotInfo& shot,
	int depth)
{
	iw::Entity entity = MakeProjectile(shot);
	Projectile* projectile = entity.Find<Projectile>();

	projectile->PlaceCell = [=](iw::SandChunk* chunk, int px, int py, float dx, float dy)
	{
		iw::Cell cell;
		cell.Type = iw::CellType::PROJECTILE;
		cell.Color = iw::Color::From255(255, 23, 6);
		cell.life = .2;

		chunk->SetCell(px, py, cell);
	};

	projectile->OnHit = [=](float fhx, float fhy)
	{

		// if return t/f, should just continue with regular path

		//auto [x, y, z, dx, dy] = getpos(entity);
		//getproj(entity)->PlaceCell(sand->m_world->GetChunk(fhx, fhy), fhx, fhy, dx, dy);

		int hx = floor(fhx);
		int hy = floor(fhy);

		if (depth < 25 && sand->m_world->InBounds(hx, hy))
		{
			ShotInfo split = shot;
			split.x = fhx;
			split.y = fhy;
			
			auto [x, y, z, dx, dy] = getpos(entity);
			float speed = sqrt(dx*dx + dy*dy);
			auto [dx2, dy2] = randvel(entity, iw::Pi/8);

			split.dx = dx2;
			split.dy = dy2;
			Bus->push<SpawnProjectile_Event>(split, depth + 1);
				
			if (iw::randf() > .9) // is there a way to calc a % to make this not diverge, i.e x % of them double
			{
				auto [dx3, dy3] = randvel(entity, iw::Pi / 6);
				split.dx = dx3;
				split.dy = dy3;
				Bus->push<SpawnProjectile_Event>(split, depth + 1);
			}
		}

		Space->QueueEntity(entity.Handle, iw::func_Destroy);
	};

	return entity;
}

iw::Entity ProjectileSystem::MakeBeam(const ShotInfo& shot, int depth)
{
	iw::Entity entity = MakeProjectile(shot);
	Projectile* projectile = entity.Find<Projectile>();

	projectile->PlaceCell = [=](iw::SandChunk* chunk, int px, int py, float dx, float dy)
	{
		iw::Color centerColor = iw::Color::From255(252, 239, 91);
		iw::Color outerColor  = iw::Color::From255(255, 66, 255);

		iw::Cell cell;
		cell.Type = iw::CellType::PROJECTILE;
		cell.Props = iw::CellProp::MOVE_FORCE;

		for (int x = -3; x < 3; x++)
		for (int y = -3; y < 3; y++)
		{
			float dist = sqrt(x*x + y*y) / sqrt(3*3+3*3);

			cell.Color = iw::lerp(centerColor.rgb(), outerColor.rgb(), dist);
			cell.dx = iw::randfs() * 10 * (dist * 2 + 1);
			cell.dy = iw::randfs() * 10 * (dist * 2 + 1);
			cell.life = iw::randf()*2;

			sand->m_world->SetCell(px, py, cell);
		}
	};

	projectile->OnHit = [=](float fhx, float fhy)
	{
		int hx = floor(fhx);
		int hy = floor(fhy);

		if (depth < 25 && sand->m_world->InBounds(hx, hy))
		{
			auto [dx2, dy2] = randvel(entity, iw::Pi/80);
			
			ShotInfo split = shot;
			split.x = fhx; // should make faster could fill line
			split.y = fhy;
			split.dx = dx2;
			split.dy = dy2;
			
			Bus->push<SpawnProjectile_Event>(split, depth); // bores forever
		}

		Space->QueueEntity(entity.Handle, iw::func_Destroy);
	};

	return entity;
}

void ProjectileSystem::MakeExplosion(
	int x, int y,
	int r)
{
	for (int px = x - r; px < x + r; px++)
	for (int py = y - r; py < y + r; py++)
	{
		if (iw::SandChunk* chunk = sand->m_world->GetChunk(px, py))
		{
			auto[tile, index] = chunk->GetCell<iw::TileInfo>(px, py, iw::SandField::TILE_INFO);

			if (tile)
			{
				sand->EjectPixel(tile, index);
			}

			chunk->SetCell(px, py, iw::Cell::GetDefault(iw::CellType::EMPTY));
		}
	}
}
