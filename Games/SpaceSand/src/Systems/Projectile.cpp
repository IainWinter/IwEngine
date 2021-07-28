#include "Systems/Projectile.h"
#include "iw/physics/Collision/SphereCollider.h"

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
}

float time__ = 0;

void ProjectileSystem::FixedUpdate()
{
	Space->Query<Projectile>().Each([&](
		iw::EntityHandle entity,
		Projectile* projectile)
	{
		if (   projectile->TestForHit 
			&& projectile->TestForHit())
		{
			LOG_INFO << "Projectile hit!";

			if (projectile->OnHit)
			{
				projectile->OnHit();
			}
		}

		if (projectile->Update)
		{
			projectile->Update();
		}
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

iw::Entity ProjectileSystem::MakeBulletC(
	float start_x, float start_y,
	float vx, float vy, 
	int maxDepth, int depth)
{
	return MakeBullet(
		start_x / sand->m_cellsPerMeter, 
		start_y / sand->m_cellsPerMeter, 
		vx / sand->m_cellsPerMeter / iw::FixedTime(), 
		vy / sand->m_cellsPerMeter / iw::FixedTime(), 
		maxDepth, depth
	);
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

		float x = t.Position.x * sand->m_cellsPerMeter;
		float y = t.Position.y * sand->m_cellsPerMeter;

		float dx = r->Velocity.x * sand->m_cellsPerMeter * iw::FixedTime();
		float dy = r->Velocity.y * sand->m_cellsPerMeter * iw::FixedTime();

		return std::array<float, 4> { x, y, dx, dy }; // tie would break elements for some reason, was it a tuple of references?
	};

	auto randvel = [=](float rand, bool setForMe) 
	{
		iw::Rigidbody* r = entity.Find<iw::Rigidbody>();
		float dx = r->Velocity.x;
		float dy = r->Velocity.y;

		float length = sqrt(dx * dx + dy * dy);
		float angle = atan2(dy, dx);
		angle += iw::randf() * rand + .1 * (signbit(iw::randf()) ? -1 : 1);

		dx = cos(angle) * length;
		dy = sin(angle) * length;

		if (setForMe) {
			r->Velocity.x = dx;
			r->Velocity.y = dy;
		}

		return std::array<float, 2> {
			dx * sand->m_cellsPerMeter * iw::FixedTime(),
			dy * sand->m_cellsPerMeter * iw::FixedTime()
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

		sand->ForEachInLine(x, y, x + dx, y + dy, [&](
			int px, int py) 
		{
			iw::SandChunk* chunk = sand->m_world->GetChunk(px, py);
			if (!chunk) return;

			chunk->SetCell(px, py, cell);

			auto& [tile, index] = chunk->GetCell<iw::TileInfo>(px, py, iw::SandField::TILE_INFO);
			if (tile)
			{
				sand->EjectPixel(tile, index);
			}

			m_cells.emplace_back(px, py, length / speed);
		});
	};

	projectile->TestForHit = [=]()
	{
		auto [x, y, dx, dy] = getpos();

		if (iw::SandChunk* chunk = sand->m_world->GetChunk(x, y))
		{
			return !chunk->IsEmpty(x, y) 
				&&  chunk->GetCell(x, y).Type != iw::CellType::PROJECTILE;
		}
	};

	projectile->OnHit = [=]()
	{
		auto [x, y, dx, dy] = getpos();

		if (sand->m_world->InBounds(x, y))
		{
			float depthPercent = depth / float(maxDepth) + iw::Pi / 6.f; // needs a lil tuning to get right feel
			int splitCount = iw::randi(2) + 1;

			for (int i = 0; i < splitCount; i++)
			{
				auto [rx, ry] = randvel(iw::Pi * depthPercent, false);
				MakeBulletC(x, y, rx, ry, maxDepth, depth + 1);
			}

			MakeExplosion(x, y, 50);
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
		if (iw::SandChunk* chunk = sand->m_world->GetChunk(x, y))
		{
			auto[tile, index] = chunk->GetCell<iw::TileInfo>(x, y, iw::SandField::TILE_INFO);

			if (tile)
			{
				sand->EjectPixel(tile, index);
			}

			//schunk->SetCell(px, py, iw::Cell::GetDefault(iw::CellType::EMPTY));
		}
	}
}
