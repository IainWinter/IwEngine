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

		auto [w, h] = sand->GetSandTexSize2();

		float offX = iw::randf() * 1;
		float offY = iw::randf() * 1;

		glm::vec2 vel(x * 2 - 1, y * 2 - 1);
		vel = glm::normalize(vel) * 44.f;
		
		Bus->push<SpawnProjectile_Event>(w, h, vel.x + offX, -vel.y + offY, SpawnProjectile_Event::BULLET, 15);
	}
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
	if (e.Action != SPAWN_PROJECTILE) return false;

	SpawnProjectile_Event& event = e.as<SpawnProjectile_Event>();

	if (event.Depth > event.MaxDepth)
	{
		return true;
	}

	iw::Entity entity = MakeProjectile(event.X, event.Y, event.dX, event.dY);
	Projectile* projectile = entity.Find<Projectile>();

	switch (event.Type) {
		case SpawnProjectile_Event::BULLET: 
		{
			projectile->PlaceCell = [=](iw::SandChunk* chunk, int px, int py, float dx, float dy)
			{
				iw::Cell cell;
				cell.Type = iw::CellType::PROJECTILE;
				cell.Color = iw::Color::From255(255, 230, 66);

				float time = .02; // not sure if this should scale with speed

				chunk->SetCell(px, py, cell);
				m_cells.emplace_back(px, py, time);
			};

			projectile->OnHit = [=](float fhx, float fhy)
			{
				int hx = floor(fhx);
				int hy = floor(fhy);

				if (sand->m_world->InBounds(hx, hy))
				{
					auto [x, y, z, dx, dy] = getpos(entity);
					float speed = sqrt(dx*dx + dy*dy);
					auto [dx2, dy2] = randvel(entity, iw::Pi/4);
					Bus->push<SpawnProjectile_Event>(fhx, fhy, dx2, dy2, event.Type, event.MaxDepth, event.Depth + 1);
				}

				Space->QueueEntity(entity.Handle, iw::func_Destroy);
			};

			break;
		}
		case SpawnProjectile_Event::LASER:
		{
			projectile->PlaceCell = [=](iw::SandChunk* chunk, int px, int py, float dx, float dy)
			{
				iw::Cell cell;
				cell.Type = iw::CellType::PROJECTILE;
				cell.Color = iw::Color::From255(255, 23, 6);

				float time = .2; // not sure if this should scale with speed

				chunk->SetCell(px, py, cell);
				m_cells.emplace_back(px, py, time);
			};

			projectile->OnHit = [=](float fhx, float fhy)
			{
				int hx = floor(fhx);
				int hy = floor(fhy);

				if (sand->m_world->InBounds(hx, hy))
				{
					auto [x, y, z, dx, dy] = getpos(entity);
					float speed = sqrt(dx*dx + dy*dy);
					auto [dx2, dy2] = randvel(entity, iw::Pi/8);
					Bus->push<SpawnProjectile_Event>(fhx, fhy, dx2, dy2, event.Type, event.MaxDepth, event.Depth + 1);
				
					if (iw::randf() > .66) // is there a way to calc a % to make this not diverge, i.e x % of them double
					{
						auto [dx3, dy3] = randvel(entity, iw::Pi / 6);
						Bus->push<SpawnProjectile_Event>(fhx, fhy, dx3, dy3, event.Type, event.MaxDepth, event.Depth + 1);
					}
				}

				Space->QueueEntity(entity.Handle, iw::func_Destroy);
			};

			break;
		}
	}

	return false;
}

iw::Entity ProjectileSystem::MakeProjectile(
	float  x, float  y,
	float dx, float dy)
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

	transform->Position = glm::vec3(x, y, 0);

	rigidbody->SetTransform(transform);
	rigidbody->Velocity = glm::vec3(dx, dy, 0);
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
				auto& [tile, index] = chunk->GetCell<iw::TileInfo>(px, py, iw::SandField::TILE_INFO);
				if (tile)
				{
					if (tile->m_zIndex == zIndex)
					{
						sand->EjectPixel(tile, index);
						hit = true;
						tile = nullptr;
						index = 0;
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
