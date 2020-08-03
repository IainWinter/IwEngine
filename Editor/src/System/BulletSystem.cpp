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

			iw::Mesh& mesh = Space->FindComponent<iw::Model>(entity)->GetMesh(0);

			switch (bullet->Type) {
				case ORBIT: mesh.SetMaterial(orbitMat); break;
				case SEEK:  mesh.SetMaterial(seekMat);  break;
			}
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

					iw::vector3 delta = (nD - nV) * 0.225f;

					rigidbody->SetVelocity((vel + delta).normalized() * bullet->Speed);
				}

				break;
			}
			case ORBIT: {
				iw::vector3 vel = rigidbody->Velocity();
				iw::vector3 dir = player.Find<iw::Transform>()->Position - transform->Position;

				iw::vector3 nV = vel.normalized();
				iw::vector3 nD = dir.normalized();

				iw::vector3 delta = (nD - nV) * 0.225f;

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
		return;

		if (package->Exploded) return;

		switch (package->Type) {
			case PackageType::TIMER : {
				if (bullet->Timer < package->TimeToExplode) {
					return;
				}
				break;
			}
			case PackageType::DISTANCE: {
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

		package->Exploded = true;

		for (float dir = 0.0f; dir < iw::Pi2; dir += iw::Pi2 / 6) {
			iw::Entity bullet = Space->Instantiate(bulletPrefab);

			Bullet*             b = bullet.Find<Bullet>();
			iw::Transform*      t = bullet.Find<iw::Transform>();
			iw::SphereCollider* s = bullet.Find<iw::SphereCollider>();
			iw::Rigidbody*      r = bullet.Find<iw::Rigidbody>();

			b->Type  = package->InnerType;
			b->Speed = package->InnerSpeed;
			b->Timer = 0.0f;

			t->SetParent(transform->Parent());

			t->Rotation = iw::quaternion::from_euler_angles(0, dir, 0).inverted();
			t->Position = transform->Position + t->Right() * sqrt(.25f);

			r->SetCol(s);
			r->SetTrans(t);
			r->SetVelocity(iw::vector3::unit_x * t->Rotation * b->Speed);

			Physics->AddRigidbody(r);
		}

		transform->SetParent(nullptr);
		Space->QueueEntity(entity, iw::func_Destroy);
	});
}
