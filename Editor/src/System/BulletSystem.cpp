#include "Systems/BulletSystem.h"

#include "Components/Bullet.h"
#include "Components/DontDeleteBullets.h"
#include "Components/EnemyDeathCircle.h"
#include "Components/LevelDoor.h"
#include "Components/Player.h"
#include "Components/Enemy.h"

#include "iw/physics/Collision/SphereCollider.h"
#include "iw/common/Components/Transform.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/graphics/Model.h"

#include "Events/ActionEvents.h"

#include "iw/events/binding.h"

BulletSystem::BulletSystem(
	iw::Entity& player)
	: iw::SystemBase("Bullet")
	, player(player)
{}

void BulletSystem::collide(iw::Manifold& man, iw::scalar dt)
{
	iw::Entity bulletEntity, otherEntity;
	bool noent = GetEntitiesFromManifold<Bullet>(man, bulletEntity, otherEntity);

	if (noent) {
		return;
	}

	Bullet* bullet = bulletEntity.Find<Bullet>();

	if (   otherEntity.Has<Bullet>()
		|| otherEntity.Has<Enemy>()
		|| otherEntity.Has<DontDeleteBullets>()
		|| otherEntity.Has<LevelDoor>())
	{
		return;
	}

	iw::Transform* bulletTransform = bulletEntity.Find<iw::Transform>();

	if (otherEntity.Has<Player>()) {
		Bus->push<GiveScoreEvent>(bulletTransform->Position, 0, true);
	}

	else if (otherEntity.Has<EnemyDeathCircle>()) {
		iw::Transform* otherTransform = otherEntity.Find<iw::Transform>();

		float score = ceil((bulletTransform->Position - otherTransform->Position).length()) * 10;
		Bus->push<GiveScoreEvent>(bulletTransform->Position, score, false);
	}

	bulletTransform->SetParent(nullptr);
	Space->DestroyEntity(bulletEntity.Index());
}

int BulletSystem::Initialize() {
	iw::Mesh mesh = Asset->Load<iw::Model>("Sphere")->GetMesh(0).MakeInstance();

	mesh.Material()->SetShader(Asset->Load<iw::Shader>("shaders/phong.shader"));
	mesh.Material()->Set("baseColor", iw::Color::From255(0, 213, 255, 191));
	mesh.Material()->Set("emissive", 2.0f);

	iw::Model model;
	model.AddMesh(mesh);

	iw::Rigidbody rigidbody;
	rigidbody.SetMass(1);
	rigidbody.SetSimGravity(false);
	rigidbody.SetIsTrigger(true);

	rigidbody.SetOnCollision(iw::bind<void, BulletSystem*, iw::Manifold&, float>(&BulletSystem::collide, this)); // garbo

	iw::SphereCollider collider(0, 0.5f);

	iw::Transform transform(0, 0.25f);

	Bullet bullet;
	bullet.Type = LINE;
	bullet.Speed = 5.0f;
	bullet.Timer = 0.0f;

	bulletPrefab.Add(Space->RegisterComponent<iw::Transform>(), &transform);
	bulletPrefab.Add(Space->RegisterComponent<iw::Rigidbody>(), &rigidbody);
	bulletPrefab.Add(Space->RegisterComponent<iw::SphereCollider>(), &collider);
	bulletPrefab.Add(Space->RegisterComponent<iw::Model>(), &model);
	bulletPrefab.Add(Space->RegisterComponent<Bullet>(), &bullet);

	return 0;
}

void BulletSystem::FixedUpdate() {
	auto bullets  = Space->Query<iw::Transform, iw::Rigidbody, Bullet>();
	auto packages = Space->Query<iw::Transform, Bullet, BulletPackage>();

	bullets.Each([&](
		auto entity,
		auto transform,
		auto rigidbody,
		auto bullet)
	{
		if (bullet->Timer == 0.0f) {
			bullet->InitialVelocity = rigidbody->Velocity();
		}

		switch (bullet->Type) {
			case SINE: {
				float speed = (sin(bullet->Timer * 5) + 1) * 0.5f;
				rigidbody->SetVelocity(bullet->InitialVelocity * speed);

				break;
			}
			case ORBIT: {
				iw::vector3 target = player.Find<iw::Transform>()->Position;
				rigidbody->ApplyForce((target - transform->Position) * 0.5f);

				break;
			}
			case SEEK: {
				iw::vector3 target = player.Find<iw::Transform>()->Position;
				rigidbody->SetVelocity((target - transform->Position).normalized() * bullet->Speed);

				break;
			}
		}

		bullet->Timer += iw::Time::FixedTime();
	});

	packages.Each([&](
		auto entity,
		auto transform,
		auto bullet,
		auto package)
	{
		if (bullet->Timer > package->TimeToExplode) {
			iw::Entity bullet = Space->Instantiate(bulletPrefab);

			bullet.Find<Bullet>()->Type  = package->InnerType;
			bullet.Find<Bullet>()->Speed = package->InnerSpeed;

			bullet.Find<iw::Transform>()->Position = transform->Position;
		}
	});
}
