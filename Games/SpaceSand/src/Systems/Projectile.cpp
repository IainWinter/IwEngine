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

void ProjectileSystem::FixedUpdate()
{
	if (iw::Keyboard::KeyDown(iw::G))
	{
		float x = iw::Mouse::ScreenPos().x() / Renderer->Width();
		float y = iw::Mouse::ScreenPos().y() / Renderer->Height();

		glm::vec2 vel(x * 2 - 1 + iw::randf() * .1, y * 2 - 1 + iw::randf() * .1);
		vel = glm::normalize(vel) * 25.f;
		
		MakeBullet(0, 0, vel.x, -vel.y);
	}

	Space->Query<Projectile>().Each([&](
		iw::EntityHandle entity,
		Projectile* projectile)
	{
		if (!projectile->Update) return;

		projectile->Update();

		if (   projectile->TestForHit 
			&& projectile->TestForHit())
		{
			LOG_INFO << "Projectile hit!";

			if (projectile->OnHit)
			{
				projectile->OnHit();
			}
		}
	});
}

iw::Entity ProjectileSystem::MakeBulletC(
	float start_x, float start_y,
	float vx, float vy, int depth)
{
	return MakeBullet(
		start_x / sand->m_cellsPerMeter, 
		start_y / sand->m_cellsPerMeter, 
		vx / sand->m_cellsPerMeter / iw::FixedTime(), 
		vy / sand->m_cellsPerMeter / iw::FixedTime(), depth
	);
}

iw::Entity ProjectileSystem::MakeBullet(
	float start_x, float start_y, 
	float vx, float vy, int depth)
{
	if (depth > 5)
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

	auto getpos = [=]() {
		iw::Rigidbody* r = entity.Find<iw::Rigidbody>();
		iw::Transform& t = r->Transform;

		float x = t.Position.x * sand->m_cellsPerMeter;
		float y = t.Position.y * sand->m_cellsPerMeter;

		float dx = r->Velocity.x * sand->m_cellsPerMeter * iw::FixedTime();
		float dy = r->Velocity.y * sand->m_cellsPerMeter * iw::FixedTime();

		return std::array<float, 4> { x, y, dx, dy }; // tie would break elements for some reason, was it a tuple of references?
	};

	projectile->Update = [=]() 
	{
		auto [x, y, dx, dy] = getpos();

		iw::Cell c = iw::Cell::GetDefault(iw::CellType::WOOD);

		float speed  = sqrt(dx*dx + dy*dy);
		float length = .2;

		sand->ForEachInLine(x, y, x + dx, y + dy, [&](
			int px, int py) 
		{
			m_cells.emplace_back(px, py, length / speed);
			sand->m_world->SetCell(px, py, c);
		});
	};

	projectile->TestForHit = [=]()
	{
		auto [x, y, dx, dy] = getpos();
		if (!sand->m_world->InBounds(x + dx, y + dy))
		{
			return true;
		}

		return sand->m_world->GetCell(x + dx, y + dy).Type == iw::CellType::ROCK;
	};

	projectile->OnHit = [=]()
	{
		auto [x, y, dx, dy] = getpos();

		if (sand->m_world->InBounds(x + dx, y + dy)) // could pass something from TestForHit to not check this twice
		{
			if (iw::randf() > 0.f) 
			{
				float length = sqrt(dx*dx + dy*dy);
				float angle = atan2(y, x);
				angle += iw::randf() * iw::Pi * 2/3.f;

				MakeBulletC(x, y, cos(angle) * length, sin(angle) * length, depth + 1);
			}

			for (int i = -2; i < 2; i++)
			for (int j = -2; j < 2; j++)
			{
				sand->m_world->SetCell(x + i, y + j, iw::Cell::GetDefault(iw::CellType::EMPTY));
			}
		}

		Space->QueueEntity(entity.Handle, iw::func_Destroy);
	};

	return entity;
}
