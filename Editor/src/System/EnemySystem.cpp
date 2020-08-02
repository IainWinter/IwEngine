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
	iw::Entity& player,
	iw::Prefab& bullet)
	: iw::System<iw::Transform, Enemy>("Enemy")
	, m_enemyCount(0)
	, m_levelResetTimer(0)
	, player(player)
	, m_bullet(bullet)
{}

void EnemySystem::Update(
	iw::EntityComponentArray& view)
{
	unsigned enemyCount = 0;

	for (auto entity : view) {
		auto [transform, enemy] = entity.Components.Tie<Components>();

		iw::CollisionObject* obj = Space->FindComponent<iw::CollisionObject>(entity.Handle);
		if (!obj) {
			obj = Space->FindComponent<iw::Rigidbody>(entity.Handle);
		}

		enemyCount++;

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
						iw::quaternion::from_euler_angles(0, enemy->Rotation, 0)
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
							iw::quaternion::from_euler_angles(0, enemy->Rotation + enemy->Speed * i, 0)
						);

						transform->Parent()->AddChild(bullet);
					}

					break;
				}
				case EnemyType::SEEK: {
					iw::Transform* bullet = SpawnBullet(
						enemy->Bullet,
						transform->Position,
						transform->Rotation.inverted()
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
										transform->Rotation.inverted() * offset
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
								transform->Rotation.inverted() * offset
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
									transform->Rotation.inverted() * offset
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
							float rot = iw::Time::TotalTime() * iw::Pi2 * 3;

							iw::quaternion offset = transform->Rotation.inverted()
								* iw::quaternion::from_euler_angles(0, rot, 0);

							Enemy child {};
							child.Type = EnemyType::SPIN;
							child.Bullet = enemy->Bullet;
							child.Speed = 0.2617994;
							child.FireTime = 0.120000;
							child.ChargeTime = 0.000000;
							child.HasShot = false;
							child.JustHit = false;

							iw::vector3 position = transform->Position + iw::vector3(sqrt(2), 1, 0) * offset;
							iw::vector3 velocity = transform->Forward() * offset;

							velocity *= 7.0f + iw::randf() * 2;
							velocity.y = 10.0f;

							Bus->push<SpawnEnemyEvent>(
								child,
								position,
								velocity,
								transform->Parent());
						}

						enemy->Timer2 += iw::Time::DeltaTimeScaled();

						if (enemy->Timer2 > 0.15f) {
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
						if (m_enemyCount > 4 || enemy->JustHit) {
							enemy->Timer = fire2;
							enemy->JustHit = false;
						}
						
						else {
							enemy->Timer = fire1;
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

	if (   enemyCount == 0
		&& m_levelResetTimer < 0.2f)
	{
		m_levelResetTimer += iw::Time::DeltaTime();
		if (m_levelResetTimer > 0.2f) {
			Bus->push<UnlockLevelDoorEvent>();
		}
	}

	m_enemyCount = enemyCount;
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

iw::Transform* EnemySystem::SpawnBullet(
	Bullet enemyBullet,
	iw::vector3 position,
	iw::quaternion rot)
{
	iw::Entity bullet = Space->Instantiate(m_bullet);
	
	if (enemyBullet.Package) {
		BulletPackage* p = bullet.Add<BulletPackage>();

		p->Type      = PackageType(enemyBullet.Package & GET_TYPE);
		p->InnerType = BulletType (enemyBullet.Package & REMOVE_TYPE);
		p->InnerSpeed    = 5.0f;
		p->TimeToExplode = 1.5f;
	}

	Bullet*             b = bullet.Find<Bullet>();
	iw::Transform*      t = bullet.Find<iw::Transform>();
	iw::SphereCollider* s = bullet.Find<iw::SphereCollider>();
	iw::Rigidbody*      r = bullet.Find<iw::Rigidbody>();

	b->Type    = enemyBullet.Type;
	b->Package = enemyBullet.Package;
	b->Speed   = enemyBullet.Speed;

	t->Rotation = rot;
	t->Position = position + t->Right() * sqrt(2);

	r->SetCol(s);
	r->SetTrans(t);
	r->SetVelocity(iw::vector3::unit_x * rot * b->Speed);

	r->SetOnCollision([&](iw::Manifold& man, float dt) { // put in prefab
		iw::Entity bulletEntity, otherEntity;
		bool noent = GetEntitiesFromManifold<Bullet>(man, bulletEntity, otherEntity);

		if (noent) {
			return;
		}

		Bullet* bullet = bulletEntity.Find<Bullet>();

		if (otherEntity.Has<Bullet>()
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
		Space->QueueEntity(bulletEntity.Handle, iw::func_Destroy);
	});

	Physics->AddRigidbody(r);

	return t;
}

iw::Transform* EnemySystem::SpawnEnemy(
	Enemy prefab,
	iw::vector3 position,
	iw::vector3 velocity)
{
	// Example using an ECS that I'll cover in another video
	// 'Physics' is the DynamicsWorld

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

		r->SetStaticFriction(.5f);
		r->SetDynamicFriction(.5f);

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

		Space->QueueChange(&enemy->JustHit, true);
		Space->QueueChange(&enemy->Health,  enemy->Health - 1);

		if (enemy->Health - 1 > 0) {
			return;
		}

		iw::Transform* transform = enemyEntity.Find<iw::Transform>();

		float score = floor(5 * (1.0f - player->Timer / player->DashTime)) * 200 + 200;

		Audio->AsStudio()->CreateInstance("enemyDeath");

		Bus->push<SpawnEnemyDeath>(transform->Position, transform->Parent());
		Bus->push<GiveScoreEvent> (transform->Position, score * enemy->ScoreMultiple);

		transform->SetParent(nullptr);
		Space->QueueEntity(enemyEntity.Handle, iw::func_Destroy);
	});

	return t;
}
