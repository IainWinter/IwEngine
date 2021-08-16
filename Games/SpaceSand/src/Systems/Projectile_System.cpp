#include "Systems/Projectile_System.h"

int ProjectileSystem::Initialize()
{
	return 0;
}

void ProjectileSystem::Update()
{
	for (int i = 0; i < m_cells.size(); i++)
	{
		auto& [x, y, life] = m_cells.at(i);

		life -= iw::DeltaTime();
		if (life < 0.f)
		{
			sand->m_world->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::EMPTY));
			m_cells.at(i) = m_cells.back(); m_cells.pop_back(); i--;
		}
	}

	Space->Query<Projectile>().Each([&](iw::EntityHandle, Projectile* p) 
	{
		if (p->Update)
		{
			auto [hit, hx, hy] = p->Update();

			if (p->OnHit && hit) 
			{
				p->OnHit(hx, hy);

				LOG_INFO << "At " << hx << ", " << hy;
			}
		}
	});
}

float time__ = 0;

void ProjectileSystem::FixedUpdate()
{
	Space->Query<Projectile>().Each([&](iw::EntityHandle, Projectile* p) 
	{
		if (p->FixedUpdate) p->FixedUpdate();
	});

	time__ += iw::FixedTime();

	if (iw::Keyboard::KeyDown(iw::G) && time__ > .05)
	{
		time__ = 0;

		float x = iw::Mouse::ScreenPos().x() / Renderer->Width();
		float y = iw::Mouse::ScreenPos().y() / Renderer->Height();

		float offX = iw::randf() * 1;
		float offY = iw::randf() * 1;

		glm::vec2 vel(x * 2 - 1, y * 2 - 1);
		vel = glm::normalize(vel) * 44.f;
		
		MakeBullet(offX, offY + 10, vel.x + offX, -vel.y + offY, 1);
	}
}

bool ProjectileSystem::On(iw::ActionEvent& e)
{
	switch (e.Action) {
		case SPAWN_PROJECTILE:
		{
			SpawnProjectile_Event& event = e.as<SpawnProjectile_Event>();
			switch (event.Type) {
				case SpawnProjectile_Event::BULLET: 
				{
					MakeBullet(event.X, event.Y, event.dX, event.dY, 2);
					break;
				}
			}

			break;
		}
	}

	return false;
}

iw::Entity ProjectileSystem::MakeBullet(
	float start_x, float start_y, 
	float vx, float vy, 
	int maxDepth, int depth)
{
	if (depth > maxDepth)
	{
		return {};
	}

	iw::Entity entity = Space->CreateEntity<
		iw::Transform, 
		iw::Rigidbody,
		iw::Circle,
		Projectile>();

	iw::Circle*    collider  = entity.Set<iw::Circle>();
	iw::Rigidbody* rigidbody = entity.Set<iw::Rigidbody>();

	rigidbody->Collider = collider;
	rigidbody->Transform.Position = glm::vec3(start_x, start_y, 0);
	rigidbody->Velocity = glm::vec3(vx, vy, 0);

	rigidbody->IsTrigger = true;

	Physics->AddRigidbody(rigidbody);

	Projectile* projectile = entity.Set<Projectile>();

	auto getpos = [=]()
	{
		iw::Rigidbody* r = entity.Find<iw::Rigidbody>();
		iw::Transform& t = r->Transform;

		float x = t.Position.x;
		float y = t.Position.y;

		float dx = r->Velocity.x * iw::FixedTime();
		float dy = r->Velocity.y * iw::FixedTime();

		return std::array<float, 4> { x, y, dx, dy }; // tie would break elements for some reason, was it a tuple of references?
	};

	auto setpos = [=](float x, float y)
	{
		iw::Rigidbody* r = entity.Find<iw::Rigidbody>();
		iw::Transform& t = r->Transform;

		t.Position.x = x;
		t.Position.y = y;

		r->Velocity = glm::normalize(r->Velocity) / iw::FixedTime();
	};

	auto randvel = [=](float amount, bool setForMe) 
	{
		iw::Rigidbody* r = entity.Find<iw::Rigidbody>();
		float dx = r->Velocity.x;
		float dy = r->Velocity.y;

		float length = sqrt(dx * dx + dy * dy);
		float angle = atan2(dy, dx);
		angle += ((iw::randf()+1) * .5f) * amount + .1f * amount;

		dx = cos(angle) * length;
		dy = sin(angle) * length;

		if (setForMe) {
			r->Velocity.x = dx;
			r->Velocity.y = dy;
		}

		return std::array<float, 2> {
			dx * iw::FixedTime(),
			dy * iw::FixedTime()
		};
	};

	projectile->Update = [=]() 
	{
		auto [x, y, dx, dy] = getpos();

		iw::Cell cell;
		cell.Type = iw::CellType::PROJECTILE;
		cell.Color = iw::Color::From255(255, 230, 66);

		float speed  = sqrt(dx*dx + dy*dy);
		float length = .2;

		bool hit = false;
		int hx, hy;

		sand->ForEachInLine(x, y, x + dx, y + dy, [&](
			int px, int py)
		{
			iw::SandChunk* chunk = sand->m_world->GetChunk(px, py);
			
			if (!chunk)
			{
				LOG_INFO << "Hit bounds";

				hit = true; // collision on no chunk
				return true;
			}

			auto& [tile, index] = chunk->GetCell<iw::TileInfo>(px, py, iw::SandField::TILE_INFO);
			if (tile)
			{
				LOG_INFO << "Hit tile " << tile;

				// issue might be the jitter in the software renderer

				hit = true;
				sand->EjectPixel(tile, index);
				tile = nullptr;
				index = 0;
			}

			else
			if (  !chunk->IsEmpty(px, py)
				&& chunk->GetCell(px, py).Type != iw::CellType::PROJECTILE)
			{
				LOG_INFO << "Hit cell";

				hit = true;
			}

			chunk->SetCell(px, py, cell);
			m_cells.emplace_back(px, py, length / speed);

			if (hit) 
			{
				hx = px;
				hy = py;
				return true;
			}

			return false;
		});

		return std::tuple(hit, hx, hy);
	};

	projectile->OnHit = [=](int hx, int hy)
	{
		if (sand->m_world->InBounds(hx, hy))
		{
			float depthPercent = depth / float(maxDepth) * iw::Pi / 2.f + iw::Pi / 6.f; // needs a lil tuning to get right feel
			int splitCount = iw::randi(2) + 1;

			//auto [vx, vy] = 

			//auto [x, y, dx, dy] = getpos();

			//LOG_INFO << 

			//randvel(iw::Pi * depthPercent, true);
			setpos(hx, hy); // set vel also?`

			//MakeBullet(hx + vx, hy + vy, vx, vy);

			//for (int i = 0; i < splitCount; i++)
			//{
			//	auto [rx, ry] = randvel(iw::Pi * depthPercent, false);
			//	MakeBulletC(x, y, rx, ry, maxDepth, depth + 1);
			//}

			//MakeExplosion(x, y, 50);
		}

		else {
			Space->QueueEntity(entity.Handle, iw::func_Destroy);
		}
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
