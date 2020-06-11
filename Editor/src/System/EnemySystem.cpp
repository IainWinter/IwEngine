#include "Systems/EnemySystem.h"

#include "iw/engine/Time.h"
#include "iw/graphics/Model.h"
#include "iw/input/Devices/Keyboard.h"

#include "iw/physics/Collision/SphereCollider.h";
#include "iw/physics/Dynamics/Rigidbody.h";
#include "iw/physics/AABB.h"

#include "iw/audio/AudioSpaceStudio.h"

#include "Components/Bullet.h"
#include "Components/Player.h"
#include "Components/LevelDoor.h"
#include "Components/DontDeleteBullets.h"
#include "Components/EnemyDeathCircle.h"

#include "Events/ActionEvents.h"

EnemySystem::EnemySystem(
	iw::Entity& player)
	: iw::System<iw::Transform, Enemy>("Enemy")
	, m_enemyCount(0)
	, m_levelResetTimer(0)
	, player(player)
{}

int EnemySystem::Initialize() {
	iw::Mesh sphere = Asset->Load<iw::Model>("Sphere")->GetMesh(0).MakeInstance();

	sphere.Material()->SetShader(Asset->Load<iw::Shader>("shaders/phong.shader"));
	sphere.Material()->Set("baseColor", iw::Color::From255(0, 213, 255, 191));
	sphere.Material()->Set("emissive", 5.0f);

	m_bulletModel.AddMesh(sphere);

	return 0;
}

void EnemySystem::Update(
	iw::EntityComponentArray& view)
{
	m_enemyCount = 0;

	for (auto entity : view) {
		auto [transform, enemy] = entity.Components.Tie<Components>();

		iw::CollisionObject* obj = Space->FindComponent<iw::CollisionObject>(entity.Handle);
		if (!obj) {
			obj = Space->FindComponent<iw::Rigidbody>(entity.Handle);
		}

		m_enemyCount++;

		switch (enemy->Type) {
			// spin to win ez
			case EnemyType::SPIN: {
				obj->Trans().Rotation *= iw::quaternion::from_euler_angles(0, -iw::Time::DeltaTimeScaled(), 0);
				break;
			}
			// charge over time and then shoot a circle
			case EnemyType::CIRCLE: {
				if (enemy->Timer < enemy->ChargeTime) {
					float speed = 1.0f / (enemy->ChargeTime - enemy->Timer);

					if (speed > 30.0f) {
						speed = 30.0f;
					}

					enemy->RotSpeed += 5 * speed * iw::Time::DeltaTimeScaled();
				}

				else {
					enemy->RotSpeed *= 1 - 20 * iw::Time::DeltaTimeScaled();
				}

				obj->Trans().Rotation *= iw::quaternion::from_euler_angles(0, enemy->RotSpeed * iw::Time::DeltaTimeScaled(), 0);

				break;
			}
			// lmao this one breaks the story
			// 
			case EnemyType::SEEK: {
				iw::vector3 target = player.Find<iw::Transform>()->Position;
				float       dir    = atan2(target.z - transform->Position.z, target.x - transform->Position.x);

				obj->Trans().Rotation = iw::quaternion::from_euler_angles(0, dir, 0);
				
				// need to slerp

				break;
			}
			// this one secretly breaks the story xd
			// spins to player quickly and does 1 / 2 firing moves
			case EnemyType::MINI_BOSS_BOX_SPIN: {
				// should rotate in same direction until at player
				if (enemy->Timer < enemy->ChargeTime) {
					iw::vector3 target = player.Find<iw::Transform>()->Position;
					float       dir    = atan2(target.z - transform->Position.z, target.x - transform->Position.x);

					obj->Trans().Rotation = iw::lerp(obj->Trans().Rotation, iw::quaternion::from_euler_angles(0, dir, 0), iw::Time::DeltaTime() * 4);
				}

				break;
			}
			case EnemyType::BOSS_FOREST: {
				if (enemy->Timer < enemy->ChargeTime) {
					iw::vector3 target = player.Find<iw::Transform>()->Position;
					float       dir = atan2(target.z - transform->Position.z, target.x - transform->Position.x);

					obj->Trans().Rotation = iw::lerp(obj->Trans().Rotation, iw::quaternion::from_euler_angles(0, dir, 0), iw::Time::DeltaTime() * 4);
				}

				break;
			}
		}

		if (  !enemy->HasShot
			&& enemy->Timer > enemy->ChargeTime) // on ground lmao
		{
			enemy->HasShot = true;

			switch (enemy->Type) {
				case EnemyType::SPIN: {
					iw::Transform* bullet = SpawnBullet(
						enemy->Bullet,
						transform->Position,
						iw::quaternion::from_euler_angles(0, enemy->Rotation, 0),
						entity.Index
					);

					transform->Parent()->AddChild(bullet);

					break;
				}
				case EnemyType::CIRCLE: {
					int count = roundf(iw::Pi2 / enemy->Speed);
					for (float i = 0; i < count; i++) {
						iw::Transform* bullet = SpawnBullet(
							enemy->Bullet,
							transform->Position,
							iw::quaternion::from_euler_angles(0, enemy->Rotation + enemy->Speed * i, 0),
							entity.Index
						);

						transform->Parent()->AddChild(bullet);
					}

					break;
				}
				case EnemyType::SEEK: {
					iw::Transform* bullet = SpawnBullet(
						enemy->Bullet,
						transform->Position,
						transform->Rotation.inverted(),
						entity.Index
					);

					transform->Parent()->AddChild(bullet);

					break;
				}
				case EnemyType::MINI_BOSS_BOX_SPIN: {
					const float time = enemy->FireTime - enemy->ChargeTime;

					const float fire1 = enemy->ChargeTime + time * 1.0f / 3.0f;
					const float fire2 = enemy->ChargeTime + time * 2.0f / 3.0f;

					const float fire12 = (fire2 - fire1) * 0.5f;

					if (enemy->Timer >= fire2)
					{
						if (enemy->Timer2 == 0.0f) {
							float rot = iw::Pi + iw::hPi * 0.5f * cos(enemy->Timer);

							int count = roundf(iw::Pi2 / enemy->Speed);
							int dontShoot = count * 0.5f;

							for (int i = 0; i <= count; i++) {
								if (i != dontShoot
									&& i != dontShoot - 1
									&& i != dontShoot + 1)
								{
									iw::quaternion offset = iw::quaternion::from_euler_angles(0, -rot + enemy->Speed * i, 0);

									iw::Transform* bullet = SpawnBullet(
										enemy->Bullet,
										transform->Position,
										transform->Rotation.inverted() * offset,
										entity.Index
									);

									transform->Parent()->AddChild(bullet);
								}
							}
						}

						enemy->Timer2 += iw::Time::DeltaTimeScaled();

						if (enemy->Timer2 > 0.2f) {
							enemy->Timer2 = 0.0f;
						}

						if (enemy->Timer <= fire2 + fire12) {
							enemy->HasShot = false; // resets hasShot if not finished kinda scuff
						}
					}

					else if (enemy->Timer >= fire1)
					{
						if (enemy->Timer2 == 0.0f) {
							float rot = enemy->Timer * iw::Pi2 * 2;
							iw::quaternion offset = iw::quaternion::from_euler_angles(0, rot, 0);

							iw::Transform* bullet = SpawnBullet(
								enemy->Bullet,
								transform->Position,
								transform->Rotation.inverted() * offset,
								entity.Index
							);

							transform->Parent()->AddChild(bullet);
						}

						enemy->Timer2 += iw::Time::DeltaTimeScaled();

						if (enemy->Timer2 > 0.02f) {
							enemy->Timer2 = 0.0f;
						}

						if (enemy->Timer <= fire1 + fire12) {
							enemy->HasShot = false;  // resets hasShot if not finished kinda scuff
						}

						else {
							enemy->Timer = fire2 + fire12;
						}
					}

					else
					{
						if (iw::randf() >= 0.0f) {
							enemy->Timer = fire1;
						}

						else {
							enemy->Timer = fire2;
						}

						enemy->HasShot = false;
					}

					break;
				}
				case EnemyType::BOSS_FOREST: {
					const float time = enemy->FireTime - enemy->ChargeTime;

					const float fire1 = enemy->ChargeTime + time * 1.0f / 3.0f;
					const float fire2 = enemy->ChargeTime + time * 2.0f / 3.0f;

					const float fire12 = (fire2 - fire1) * 0.5f;

					int count = roundf(iw::Pi2 / enemy->Speed);

					int dontShoot = count * 0.5f;

					if (enemy->Timer >= fire2)
					{
						if (enemy->Timer2 == 0.0f) {
							float rot = iw::Pi + iw::hPi * 0.5f * cos(enemy->Timer);

							for (int i = 0; i < count; i++) {
								iw::quaternion offset = iw::quaternion::from_euler_angles(0, -rot + enemy->Speed * i, 0);

								iw::Transform* bullet = SpawnBullet(
									enemy->Bullet,
									transform->Position,
									transform->Rotation.inverted() * offset,
									entity.Index
								);

								transform->Parent()->AddChild(bullet);
							}
						}

						enemy->Timer2 += iw::Time::DeltaTimeScaled();

						if (enemy->Timer2 > 0.1f) {
							enemy->Timer2 = 0.0f;
						}

						if (enemy->Timer <= fire2 + fire12) {
							enemy->HasShot = false; // resets hasShot if not finished kinda scuff
						}
					}

					else if (enemy->Timer >= fire1)
					{
						if (enemy->Timer2 == 0.0f) {
							float rot = enemy->Timer * iw::Pi2 * 4;

							iw::quaternion offset = transform->Rotation.inverted()
								* iw::quaternion::from_euler_angles(0, rot, 0);

							Enemy child {};
							child.Type = EnemyType::SPIN;
							child.Bullet = enemy->Bullet;
							child.Speed = 0.2617994;
							child.FireTime = 0.120000;
							child.ChargeTime = 0.000000;
							child.HasShot = false;

							iw::vector3 position = transform->Position + iw::vector3(sqrt(2), 0, 0) * offset;
							iw::vector3 velocity = transform->Forward() * offset;

							velocity *= 5 + iw::randf();
							velocity.y = 15;

							Bus->push<SpawnEnemyEvent>(
								child,
								position,
								velocity,
								transform->Parent());
						}

						enemy->Timer2 += iw::Time::DeltaTimeScaled();

						if (enemy->Timer2 > 0.1f) {
							enemy->Timer2 = 0.0f;
						}

						if (enemy->Timer <= fire1 + 0.4f) {
							enemy->HasShot = false; // resets hasShot if not finished kinda scuff
						}

						else {
							enemy->Timer = fire2 + fire12;
						}
					}

					else
					{
						if (iw::randf() >= 0.0f) {
							enemy->Timer = fire1;
						}

						else {
							enemy->Timer = fire2;
						}

						enemy->HasShot = false;
					}
				}
			}

			enemy->Rotation = fmod(enemy->Rotation + enemy->Speed, iw::Pi2);
		}

		else if (enemy->Timer >= enemy->FireTime) {
			enemy->HasShot = false;
			enemy->Timer = 0;
			enemy->Timer2 = 0;
		}

		enemy->Timer += iw::Time::DeltaTimeScaled();
	}

	if (   m_enemyCount == 0
		&& m_levelResetTimer < 0.2f)
	{
		m_levelResetTimer += iw::Time::DeltaTime();
		if (m_levelResetTimer > 0.2f) {
			Bus->push<UnlockLevelDoorEvent>();
		}
	}
}

bool EnemySystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::START_LEVEL):
		case iw::val(Actions::RESET_LEVEL): {
			m_levelResetTimer = 0;
			break;
		}
		case iw::val(Actions::SPAWN_ENEMY): {
			SpawnEnemyEvent& event = e.as<SpawnEnemyEvent>();
			iw::Transform* trans = SpawnEnemy(event.Enemy, event.Position, event.Velocity);
			event.Level->AddChild(trans);
			break;
		}
	}

	return false;
}

iw::Transform* EnemySystem::SpawnBullet( // this should be in bullet system i guess
	Bullet prefab,
	iw::vector3 position,
	iw::quaternion rot,
	int index)
{
	iw::Entity bullet = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, iw::Rigidbody, Bullet>();
	
	                        bullet.Set<iw::Model>         (m_bulletModel);
	Bullet*             b = bullet.Set<Bullet>            (prefab);
	iw::Transform*      t = bullet.Set<iw::Transform>     (position + iw::vector3(sqrt(2), 0, 0) * rot, iw::vector3(.25f));
	iw::SphereCollider* s = bullet.Set<iw::SphereCollider>(iw::vector3::zero, 0.5f);
	iw::Rigidbody*      r = bullet.Set<iw::Rigidbody>     ();

	b->initialVelocity = iw::vector3::unit_x * rot * prefab.Speed;

	b->enemyIndex = index;

	r->SetMass(1);
	r->SetCol(s);
	r->SetTrans(t);
	r->SetVelocity(b->initialVelocity);
	r->SetSimGravity(false);
	r->SetRestitution(0.1f);
	r->SetIsTrigger(true);
	//r->SetIsLocked(iw::vector3(0, 1, 0));
	//r->SetLock(iw::vector3(0, 1, 0));

	r->SetOnCollision([&](iw::Manifold& man, float dt) {
		iw::Entity bulletEntity, otherEntity;
		bool noent = GetEntitiesFromManifold<Bullet>(man, bulletEntity, otherEntity);

		if (noent) {
			return;
		}

		Bullet* bullet = bulletEntity.Find<Bullet>();
		//if (bullet->Die) return;

		if (   otherEntity.Has<Bullet>()
			|| otherEntity.Has<Enemy>()/*.Index() == bullet.Find<Bullet>()->enemyIndex*/
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
			iw::Transform* otherTransform  = otherEntity.Find<iw::Transform>();

			float score = ceil((bulletTransform->Position - otherTransform->Position).length()) * 10;
			Bus->push<GiveScoreEvent>(bulletTransform->Position, score, false);
		}

		/*bullet->Die = true;*/

		bulletTransform->SetParent(nullptr);
		Space->DestroyEntity(bulletEntity.Index());
		//Bus->push<iw::EntityDestroyEvent>(bulletEntity);
	});

	Physics->AddRigidbody(r);

	return t;
}

iw::Transform* EnemySystem::SpawnEnemy(
	Enemy prefab,
	iw::vector3 position,
	iw::vector3 velocity)
{
	iw::Entity ent = Space->CreateEntity<iw::Transform, iw::Model, iw::SphereCollider, Enemy>();

	iw::CollisionObject* c;

	if (velocity.length_squared() == 0) {
		c = ent.Add<iw::CollisionObject>();
		Physics->AddCollisionObject(c);
	}

	else {
		iw::Rigidbody* r = ent.Add<iw::Rigidbody>();
		Physics->AddRigidbody(r);
		
		r->SetVelocity(velocity);
		r->SetIsStatic(false);
		r->SetSimGravity(true);

		c = r;
	}

	Enemy*               e = ent.Set<Enemy>(prefab);
	iw::Transform*       t = ent.Set<iw::Transform>(position);
	iw::SphereCollider*  s = ent.Set<iw::SphereCollider>(iw::vector3::zero, 1.0f);
	
	iw::Model* m;
	
	switch (prefab.Type) {
		case EnemyType::BOSS_FOREST:
		case EnemyType::MINI_BOSS_BOX_SPIN: {
			e->Health = 3;
			e->ScoreMultiple = 10;
			t->Scale = 0.75f;
			m = ent.Set<iw::Model>(*Asset->Load<iw::Model>("Box"));

			break;
		}
		default: {
			e->Health = 1;
			e->ScoreMultiple = 1;
			m = ent.Set<iw::Model>(*Asset->Load<iw::Model>("Tetrahedron"));
			break;
		}
	}

	c->SetCol(s);
	c->SetTrans(t);
	c->SetIsStatic(false);
	
	c->SetOnCollision([&](iw::Manifold& man, float dt) {
		iw::Entity enemyEntity, playerEntity;
		bool noent = GetEntitiesFromManifold<Enemy, Player>(man, enemyEntity, playerEntity);

		if (noent) {
			return;
		}

		Enemy*  enemy  = enemyEntity .Find<Enemy>();
		Player* player = playerEntity.Find<Player>();

		if (   player->Transition
			|| player->Timer <= 0.0f)
		{
			return;
		}

		QueueChange(&enemy->Health, enemy->Health - 1);

		if (enemy->Health - 1 > 0) {
			return;
		}

		iw::Transform* transform = enemyEntity.Find<iw::Transform>();

		float score = floor(5 * (1.0f - player->Timer / player->DashTime)) * 200 + 200;

		Audio->AsStudio()->CreateInstance("enemyDeath");

		Bus->push<SpawnEnemyDeath>(transform->Position, transform->Parent());
		Bus->push<GiveScoreEvent> (transform->Position, score * enemy->ScoreMultiple);

		transform->SetParent(nullptr);
		Space->DestroyEntity(enemyEntity.Index());
	});

	return t;
}
