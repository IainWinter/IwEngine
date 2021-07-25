#include "Systems/Projectile.h"
#include "iw/physics/Collision/SphereCollider.h"

int ProjectileSystem::Initialize()
{
	MakeBullet(5, 5, 5, 5);

	return 0;
}

void ProjectileSystem::FixedUpdate()
{
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

			Space->QueueEntity(entity, iw::func_Destroy);
		}
	});
}

iw::Entity ProjectileSystem::MakeBulletC(
	float start_x, float start_y,
	float vx, float vy)
{
	return MakeBullet(
		start_x / sand->m_cellsPerMeter, 
		start_y / sand->m_cellsPerMeter, 
		vx / sand->m_cellsPerMeter / iw::FixedTime(), 
		vy / sand->m_cellsPerMeter / iw::FixedTime()
	);
}

iw::Entity ProjectileSystem::MakeBullet(
	float start_x, float start_y, 
	float vx, float vy)
{
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
		sand->m_world->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::ROCK));
	};

	projectile->TestForHit = [=]()
	{
		auto [x, y, dx, dy] = getpos();
		return sand->m_world->GetCell(x + dx, y + dy).Type == iw::CellType::ROCK;
	};

	projectile->OnHit = [=]()
	{
		auto [x, y, dx, dy] = getpos();
		//MakeBulletC(x, y, dx + iw::randf() * 1, dy + iw::randf() * 1);

		sand->m_world->SetCell(x, y, iw::Cell::GetDefault(iw::CellType::FIRE));
	};

	return entity;
}
