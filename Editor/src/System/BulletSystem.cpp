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
	, baseColor(iw::Color::From255(0, 213, 255, 191))
{}

// sucks that this needs to be here, need to fix lambdas to get it to work
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
	mesh.Material()->Set("baseColor", baseColor);
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
			case ORBIT: {
				iw::vector3 vel = rigidbody->Velocity();
				iw::vector3 dir = player.Find<iw::Transform>()->Position - transform->Position;

				iw::vector3 nV = vel.normalized();
				iw::vector3 nD = dir.normalized();

				float dot = nV.dot(nD);

				float rotSpeed = 10;
				if (dot < 0) {
					rotSpeed = -10;
				}

				else if (dot > .99f) {
					break;
				}

				rigidbody->SetVelocity(vel * iw::quaternion::from_euler_angles(0, iw::Time::DeltaTime() * rotSpeed, 0));

				//t->Rotation *= iw::quaternion::from_euler_angles(0, 10, 0);

				//iw::vector3 target = player.Find<iw::Transform>()->Position;

				//iw::vector3 force = target - transform->Position;
				////force.x *= 0.5f;
				////force.z *= 0.5f;

				//rigidbody->ApplyForce(force);

				//if (transform->Position.y < 1) {
				//	//iw::vector3 correction = rigidbody->Force();
				//	//correction.x = 0;
				//	//correction.z = 0;

				//	//rigidbody->ApplyForce(-correction * (target.y - transform->Position.y));

				//	rigidbody->Trans().Position.y = 1;
				//}

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
