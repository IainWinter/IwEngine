#include "Systems/RiverRaftEnemySystem.h"
#include "Events/ActionEvents.h"
#include "Components/Player.h"
#include "Components/Enemy.h"

#include "iw/graphics/Model.h"
#include "iw/physics/Collision/CollisionObject.h"
#include "iw/physics/Collision/SphereCollider.h"

#include <vector>

RiverRaftEnemySystem::RiverRaftEnemySystem()
	: iw::SystemBase("River raft enemy")
	, m_running(true)
	, m_currentLevel(nullptr)
{}

int RiverRaftEnemySystem::Initialize()
{
	m_running = true;

	Task->queue([&]() {
		Bullet bullet;
		bullet.Type = BulletType::LINE;
		bullet.Speed = 5;
		bullet.Package = PackageType::NONE;

		Enemy enemy;
		enemy.Type = EnemyType::SPIN;
		enemy.Speed = 0.2617994;
		enemy.FireTime = 0.12f;
		enemy.ChargeTime = 0;
		enemy.Rotation = iw::randf() * iw::Pi;
		enemy.Bullet = bullet;

		float wait = 3.0f;

		while (m_running) {
			float start = iw::TotalTime();

			while (iw::TotalTime() - start < wait) {} // spin lock

			if (  !m_currentLevel
				|| m_path.size() == 0) continue; // level not yet started or no path

			Bus->push<SpawnEnemyEvent>(enemy, m_path.at(0), 0, m_currentLevel);
		}
	});

	return 0;
}

void RiverRaftEnemySystem::Update() {
	Space->Query<RaftEnemy>().Each([&](
		auto e,
		RaftEnemy* raft)
	{
		iw::CollisionObject* c = GetPhysicsComponent(e);

		if(m_path.size() > raft->PathIndex) {
			iw::vector3 a = m_path.at(raft->PathIndex);
			iw::vector3 b = m_path.at(raft->PathIndex + 1);

			c->Trans().Position = iw::lerp(a, b, raft->Timer);
			
			//c->Trans().Position.y = sin(iw::TotalTime() * 2);

			raft->Timer += iw::DeltaTime() / ((b - a).length() / raft->Speed);

			if (raft->Timer > 1.0f) {
				raft->Timer = 0.0f;
				raft->PathIndex++;

				if (raft->PathIndex == m_path.size() - 1) {
					Space->QueueEntity(e, iw::func_Destroy);
				}
			}
		}
	});
}

bool RiverRaftEnemySystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::START_LEVEL): {
			StartLevelEvent& event = e.as<StartLevelEvent>();
			
			m_path.clear();

			if (event.LevelName.find("river01") != std::string::npos) {
				m_path.push_back(iw::vector3(11, -10,  2));
				m_path.push_back(iw::vector3(12, -5,  -2));
				m_path.push_back(iw::vector3(16, -3,  -3));
				m_path.push_back(iw::vector3(36,  1,   0));
			}

			else if (event.LevelName.find("river02") != std::string::npos) {
				m_path.push_back(iw::vector3(11 - 64, -10, 2));
				m_path.push_back(iw::vector3(12 - 64, -5, -2));
				m_path.push_back(iw::vector3(16 - 64, -3, -3));
				m_path.push_back(iw::vector3(36 - 64,  1,  0));

				m_path.push_back(iw::vector3( 36, 1, 0));
			}

			Space->Query<RaftEnemy>().Each([&](auto e, auto) {
				Space->QueueEntity(e, iw::func_Destroy);
			});

			m_currentLevel = event.Level;

			break;
		}
		case iw::val(Actions::SPAWN_ENEMY): {
			SpawnEnemyEvent& event = e.as<SpawnEnemyEvent>();
			if (!event.SpawnedEnemy) {
				LOG_WARNING << "The raft enemy system needs to run after the enemy system!";
				return false;
			}

			iw::Transform* transform = event.SpawnedEnemy.Find<iw::Transform>();
			if (transform->Position != m_path.at(0)) { // this is bad but works for now
				return false; // exit if not spawned at beginning of path, aka not a raft enemy
			}

			iw::CollisionObject* c = GetPhysicsComponent(event.SpawnedEnemy.Handle);
			Physics->RemoveCollisionObject(c); // make this happen automatically, add event for event changed in Space

			event.SpawnedEnemy.Add<RaftEnemy>();

			c = GetPhysicsComponent(event.SpawnedEnemy.Handle);
			c->SetCol(event.SpawnedEnemy.Find<iw::SphereCollider>());

			Physics->AddCollisionObject(c);

			break;
		}
	}

	return false;
}
