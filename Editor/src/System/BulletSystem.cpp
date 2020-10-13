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

void BulletSystem::collide(iw::Manifold& man, float dt) {
	iw::Entity bulletEntity, otherEntity;
	if (GetEntitiesFromManifold<Bullet>(man, bulletEntity, otherEntity)) {
		return;
	}

	iw::CollisionObject* o = otherEntity.Find<iw::CollisionObject>();
	if (!o) {
		o = otherEntity.Find<iw::Rigidbody>();
	}

	Bullet* bullet = bulletEntity.Find<Bullet>();

	if (   otherEntity.Has<Bullet>()
		|| otherEntity.Has<Enemy>()
		|| otherEntity.Has<DontDeleteBullets>()
		|| otherEntity.Has<LevelDoor>()
		|| (!otherEntity.Has<EnemyDeathCircle>() && o->IsTrigger()))
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

	else if (bullet->Package) {
		SpawnBulletsFromPackage(
			bulletEntity.Find<iw::Transform>(),
			bulletEntity.Find<BulletPackage>()
		);
	}

	bulletTransform->SetParent(nullptr);
	Space->QueueEntity(bulletEntity.Handle, iw::func_Destroy);
}

int BulletSystem::Initialize() {
	iw::Mesh mesh = Asset->Load<iw::Model>("Sphere")->GetMesh(0).MakeInstance();

	normalMat = mesh.Material();

	normalMat->SetShader(Asset->Load<iw::Shader>("shaders/phong.shader"));
	normalMat->Set("baseColor", iw::Color::From255(0, 213, 255, 191));
	normalMat->Set("emissive", 2.0f);

	orbitMat = normalMat->MakeInstance();
	seekMat  = normalMat->MakeInstance();

	orbitMat->Set("baseColor", iw::Color::From255(245, 134, 0, 191));
	seekMat->Set("baseColor", iw::Color::From255(245, 0, 196, 191));

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
	auto packages = Space->Query<iw::Transform, iw::Model, Bullet, BulletPackage>();
	
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
				float speed = (sin(bullet->Timer * 5) + 1) * 0.75f;
				rigidbody->SetVelocity(bullet->InitialVelocity * speed);

				break;
			}
			case SEEK: {
				if (bullet->Timer < 1.5f) {
					iw::vector3 vel = rigidbody->Velocity();
					iw::vector3 dir = player.Find<iw::Transform>()->Position - transform->Position;

					iw::vector3 nV = vel.normalized();
					iw::vector3 nD = dir.normalized();

					iw::vector3 delta = (nD - nV) * bullet->Speed * 0.045f * 2;

					rigidbody->SetVelocity((vel + delta).normalized() * bullet->Speed);
				}

				break;
			}
			case ORBIT: {
				iw::vector3 vel = rigidbody->Velocity();
				iw::vector3 dir = player.Find<iw::Transform>()->Position - transform->Position;

				iw::vector3 nV = vel.normalized();
				iw::vector3 nD = dir.normalized();

				iw::vector3 delta = (nD - nV) * bullet->Speed * 0.045;

				rigidbody->SetVelocity((vel + delta).normalized() * bullet->Speed);

				break;
			}

		}

		bullet->Timer += iw::Time::FixedTime();
	});

	packages.Each([&](
		auto entity,
		auto transform,
		auto model,
		auto bullet,
		auto package)
	{
		if (package->Exploded) return;

		switch (package->Type) {
			case PackageType::TIMER: {
				if (bullet->Timer < package->TimeToExplode) {
					return;
				}
				break;
			}
			case PackageType::DISTANCE: {
				if (bullet->Timer < 0.7) {
					return;
				}

				if ((transform->WorldPosition() - player.Find<iw::Transform>()->WorldPosition()).length_squared() > 25) {
					return;
				}
				break;
			}
			default: {
				LOG_WARNING << "Invalid bullet package type! " << package->Type;
				return;
			}
		}

		SpawnBulletsFromPackage(transform, package);

		package->Exploded = true;

		transform->SetParent(nullptr);
		Space->QueueEntity(entity, iw::func_Destroy);
	});
}

bool BulletSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::SPAWN_BULLET): {
			SpawnBulletEvent& event = e.as<SpawnBulletEvent>();
			iw::Transform* trans = SpawnBullet(event.Bullet, event.Position, event.Rotation);
			
			if (trans) {
				event.Level->AddChild(trans);
			}

			break;
		}
	}

	return false;
}

iw::Transform* BulletSystem::SpawnBullet(
	Bullet enemyBullet,
	iw::vector3 position,
	iw::quaternion rot,
	float dist)
{
	iw::Entity bullet = Space->Instantiate(bulletPrefab);

	if (enemyBullet.Package) {
		BulletPackage* p = bullet.Add<BulletPackage>();

		p->Type     = PackageType(enemyBullet.Package & GET_TYPE);
		p->InnerType = BulletType(enemyBullet.Package & REMOVE_TYPE);
		p->InnerSpeed = 5.0f;
		p->TimeToExplode = 1.8f;
	}

	Bullet*             b = bullet.Find<Bullet>();
	iw::Transform*      t = bullet.Find<iw::Transform>();
	iw::SphereCollider* s = bullet.Find<iw::SphereCollider>();
	iw::Rigidbody*      r = bullet.Find<iw::Rigidbody>();

	b->Type    = enemyBullet.Type;
	b->Package = enemyBullet.Package;
	b->Speed   = enemyBullet.Speed;
	b->Package = enemyBullet.Package;

	t->Rotation = rot;
	t->Position = position + t->Right() * sqrt(dist);

	r->SetCol(s);
	r->SetTrans(t);
	r->SetVelocity(iw::vector3::unit_x * rot * b->Speed);

	//if (Physics->TestObject(r)) {
	//	bullet.Destroy();
	//	return nullptr;
	//}

	Physics->AddRigidbody(r);

	iw::Mesh& mesh = bullet.Find<iw::Model>()->GetMesh(0);

	switch (b->Type) {
		case ORBIT: mesh.SetMaterial(orbitMat); break;
		case SEEK:  mesh.SetMaterial(seekMat);  break;
	}

	return t;
}

void BulletSystem::SpawnBulletsFromPackage(
	iw::Transform* transform,
	BulletPackage* package)
{
	for (float dir = 0.0f; dir < iw::Pi2; dir += iw::Pi2 / 5) {
		Bullet bullet;
		bullet.Type  = package->InnerType;
		bullet.Speed = package->InnerSpeed;

		iw::Transform* t = SpawnBullet(
			bullet,
			transform->Position,
			iw::quaternion::from_euler_angles(0, dir, 0).inverted(),
			0.25f
		);

		if (transform && transform->Parent()) {
			t->SetParent(transform->Parent());
		}
	}
}
