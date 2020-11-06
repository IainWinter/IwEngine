#include "Systems/RiverRaftEnemySystem.h"
#include "Events/ActionEvents.h"

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

	Bullet bullet;
	bullet.Type = BulletType::LINE;
	bullet.Speed = 5;
	bullet.Package = PackageType::NONE;

	m_spin.Type = EnemyType::SPIN;
	m_spin.Speed = 0.2617994;
	m_spin.FireTime = 0.12f;
	m_spin.ChargeTime = 0;
	m_spin.Rotation = iw::randf() * iw::Pi;
	m_spin.Bullet = bullet;

	Task->queue([&]() {
		float wait = 3.0f;

		while (m_running) {
			float start = iw::TotalTime();

			while (iw::TotalTime() - start < wait) {} // spin lock

			if (  !m_currentLevel
				|| m_path.size() == 0) continue; // level not yet started or no path

			Bus->push<SpawnEnemyEvent>(m_spin, m_path.at(0), 0, m_currentLevel);
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

			raft->Timer += iw::DeltaTimeScaled() / ((b - a).length() / raft->Speed);

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

				m_path.push_back(iw::vector3(36, 1, 0));
			}

			else if (event.LevelName.find("river03") != std::string::npos) {
				m_path.push_back(iw::vector3(36 - 86, 1, 0));
				m_path.push_back(iw::vector3(36 - 64, 1, 0));

				m_path.push_back(iw::vector3(36, 1, 0));
			}

			else if (event.LevelName.find("river04") != std::string::npos) {
				m_path.push_back(iw::vector3(36 - 86, 1, 0));
				m_path.push_back(iw::vector3(36 - 64, 1, 0));

				m_path.push_back(iw::vector3(-8, 1, -2));
				m_path.push_back(iw::vector3(-9, 1, -15.5f));
				m_path.push_back(iw::vector3(10, 1, -15.5f));
				m_path.push_back(iw::vector3(25, 1, -2));
				m_path.push_back(iw::vector3(36, 1, 0));
			}

			else if (event.LevelName.find("river05") != std::string::npos) {
				m_path.push_back(iw::vector3(10 - 64, 1, -15.5f));
				m_path.push_back(iw::vector3(25 - 64, 1, -2));
				m_path.push_back(iw::vector3(36 - 64, 1, 0));

				m_path.push_back(iw::vector3(36, 1, 0));
			}

			Space->Query<RaftEnemy>().Each([&](auto e, auto) {
				Space->QueueEntity(e, iw::func_Destroy);
			});

			m_currentLevel = event.Level;

			//for (iw::vector3 pos : m_path) {
			//	Bus->push<SpawnEnemyEvent>(m_spin, pos, 0, m_currentLevel);
			//}

			break;
		}
		case iw::val(Actions::SPAWN_ENEMY): {
			SpawnEnemyEvent& event = e.as<SpawnEnemyEvent>();
			if (!event.SpawnedEnemy) {
				LOG_WARNING << "The raft enemy system needs to run after the enemy system!";
				return false;
			} 

			auto itr = std::find(m_path.begin(), m_path.end(), event.Position);

			if (itr == m_path.end()) { // this is bad but works for now
				return false; // exit if not spawned on a path node, aka not a raft enemy
			}

			iw::CollisionObject* c = GetPhysicsComponent(event.SpawnedEnemy.Handle);
			Physics->RemoveCollisionObject(c); // make this happen automatically, add event for event changed in Space

			RaftEnemy* r = event.SpawnedEnemy.Add<RaftEnemy>();

			r->PathIndex = std::distance(m_path.begin(), itr);

			c = GetPhysicsComponent(event.SpawnedEnemy.Handle);
			c->SetCol(event.SpawnedEnemy.Find<iw::SphereCollider>());
			Physics->AddCollisionObject(c);

			break;
		}
	}

	return false;
}
