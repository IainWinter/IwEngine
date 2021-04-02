#include "Systems/RiverRaftEnemySystem.h"
#include "Events/ActionEvents.h"

#include "iw/physics/Collision/CollisionObject.h"
#include "iw/physics/Collision/SphereCollider.h"

#include <vector>

RiverRaftEnemySystem::RiverRaftEnemySystem()
	: iw::SystemBase("River raft enemy")
	, m_currentLevel(nullptr)
{}

int RiverRaftEnemySystem::Initialize() {
	m_start = iw::TotalTime();

	m_line.Type = BulletType::LINE;
	m_line.Speed = 5;
	m_line.Package = PackageType::NONE;

	m_seek = m_line;
	m_seek.Type = BulletType::SEEK;

	m_orbit = m_line;
	m_orbit.Type = BulletType::ORBIT;

	m_spin.Type = EnemyType::SPIN;
	m_spin.Speed = 0.2617994;
	m_spin.FireTime = 0.12f;
	m_spin.ChargeTime = 0;
	m_spin.Rotation = iw::randf() * iw::Pi;
	m_spin.Bullet = m_line;

	return 0;
}

void RiverRaftEnemySystem::Update() {
	Space->Query<RaftEnemy>().Each([&](
		auto e,
		RaftEnemy* raft)
	{
		iw::CollisionObject* c = GetPhysicsComponent(e);

		if (m_paths.size() <= raft->PathIndex) return;

		auto& path = m_paths.at(raft->PathIndex);

		if (path.size() <= raft->IntraPathIndex) return;

		iw::vector3 a = path.at(raft->IntraPathIndex);
		iw::vector3 b = path.at(raft->IntraPathIndex + 1);

		c->Trans().Position = iw::lerp(a, b, raft->Timer);
		
		raft->Timer += iw::DeltaTimeScaled() / ((b - a).length() / raft->Speed);

		if (raft->Timer > 1.0f) {
			raft->Timer = 0.0f;
			raft->IntraPathIndex++;

			if (raft->IntraPathIndex == path.size() - 1) {
				Space->QueueEntity(e, iw::func_Destroy);
			}
		}
	});

	float wait = 3.0f;

	if (iw::TotalTime() - m_start > wait) {
		for (unsigned i = 0; i < m_paths.size(); i++) {
			if (   !m_currentLevel
				|| m_paths.at(i).size() == 0
				|| iw::randf() > .2f) continue; // level not yet started or no path

			Enemy enemy = m_spin;

			if (i > 0 && iw::randf() > 0) {
				enemy.Bullet = m_seek;
				enemy.FireTime *= 2;
				enemy.Speed *= 2;
			}

			if (i > 1 && iw::randf() > 0) {
				enemy.Bullet = m_orbit;
				enemy.FireTime *= 4;
				enemy.Speed *= 4;
			}

			if (iw::randf() < 0) {
				enemy.Speed *= -1;
			}

			Bus->push<SpawnEnemyEvent>(enemy, m_paths.at(i).at(0), 0, m_currentLevel);
		}

		m_start = iw::TotalTime();
	}
}

bool RiverRaftEnemySystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::START_LEVEL): {
			StartLevelEvent& event = e.as<StartLevelEvent>();
			
			m_paths.clear();
			m_paths.emplace_back();

			if (event.LevelName.find("river01") != std::string::npos) {
				m_paths.at(0).push_back(iw::vector3(11, -10,  2));
				m_paths.at(0).push_back(iw::vector3(12, -5,  -2));
				m_paths.at(0).push_back(iw::vector3(16, -3,  -3));
				m_paths.at(0).push_back(iw::vector3(36,  1,   0));
			}

			else if (event.LevelName.find("river02") != std::string::npos) {
				m_paths.at(0).push_back(iw::vector3(11 - 64, -10, 2));
				m_paths.at(0).push_back(iw::vector3(12 - 64, -5, -2));
				m_paths.at(0).push_back(iw::vector3(16 - 64, -3, -3));
				m_paths.at(0).push_back(iw::vector3(36 - 64,  1,  0));

				m_paths.at(0).push_back(iw::vector3(36, 1, 0));
			}

			else if (event.LevelName.find("river03") != std::string::npos) {
				m_paths.at(0).push_back(iw::vector3(36 - 86, 1, 0));
				m_paths.at(0).push_back(iw::vector3(36 - 64, 1, 0));

				m_paths.at(0).push_back(iw::vector3(36, 1, 0));
			}

			else if (event.LevelName.find("river04") != std::string::npos) {
				m_paths.at(0).push_back(iw::vector3(36 - 86, 1, 0));
				m_paths.at(0).push_back(iw::vector3(36 - 64, 1, 0));

				m_paths.at(0).push_back(iw::vector3(-8, 1, -2));
				m_paths.at(0).push_back(iw::vector3(-9, 1, -15.5f));
				m_paths.at(0).push_back(iw::vector3(10, 1, -15.5f));
				m_paths.at(0).push_back(iw::vector3(25, 1, -2));
				m_paths.at(0).push_back(iw::vector3(36, 1, 0));
			}

			else if (event.LevelName.find("river05") != std::string::npos) {
				m_paths.at(0).push_back(iw::vector3(10 - 64, 1, -15.5f));
				m_paths.at(0).push_back(iw::vector3(25 - 64, 1, -2));
				m_paths.at(0).push_back(iw::vector3(36 - 64, 1, 0));

				m_paths.at(0).push_back(iw::vector3(36, 1, 0));
			}

			else if (event.LevelName.find("river06") != std::string::npos) {
				m_paths.at(0).push_back(iw::vector3(36 - 86, 1, 0));
				m_paths.at(0).push_back(iw::vector3(36 - 64, 1, 0));

				m_paths.at(0).push_back(iw::vector3(36, 1, 0));

				m_paths.emplace_back();
				m_paths.at(1).push_back(iw::vector3(20,  1, 32));
				m_paths.at(1).push_back(iw::vector3(17,  1, 21));
				m_paths.at(1).push_back(iw::vector3(-7,  1, 17));
				m_paths.at(1).push_back(iw::vector3(-10, 1, 5));
				m_paths.at(1).push_back(iw::vector3(-5.5f,  1, 0));
				m_paths.at(1).push_back(iw::vector3(36,  1, 0));
			}

			else if (event.LevelName.find("river07") != std::string::npos) {
				m_paths.at(0).push_back(iw::vector3(36 - 86, 1, 0));
				m_paths.at(0).push_back(iw::vector3(36 - 64, 1, 0));

				m_paths.at(0).push_back(iw::vector3(36, 1, 0));

				m_paths.emplace_back();
				m_paths.at(1).push_back(iw::vector3(36 - 102, 1, 0));
				m_paths.at(1).push_back(iw::vector3(36 - 86, 1, 0));
				m_paths.at(1).push_back(iw::vector3(36 - 64, 1, 0));

				m_paths.at(1).push_back(iw::vector3(36, 1, 0));

				m_paths.emplace_back();
				m_paths.at(2).push_back(iw::vector3(-4.5f, -3, -31));
				m_paths.at(2).push_back(iw::vector3(-4, 1, -24));
				m_paths.at(2).push_back(iw::vector3(-3, 1, -19));
				m_paths.at(2).push_back(iw::vector3(1.5f, 1, -6));
				m_paths.at(2).push_back(iw::vector3(8, 1, 0));
				m_paths.at(2).push_back(iw::vector3(36, 1, 0));
			}

			else if (event.LevelName.find("river08") != std::string::npos) {
				m_paths.at(0).push_back(iw::vector3(36 - 86, 1, 0));
				m_paths.at(0).push_back(iw::vector3(36 - 64, 1, 0));

				m_paths.at(0).push_back(iw::vector3(36, 1, 0));

				m_paths.emplace_back();
				m_paths.at(1).push_back(iw::vector3(36 - 116, 1, 0));
				m_paths.at(1).push_back(iw::vector3(36 - 64, 1, 0));

				m_paths.at(1).push_back(iw::vector3(36, 1, 0));

				m_paths.emplace_back();
				m_paths.at(2).push_back(iw::vector3(36 - 122, 1, 0));
				m_paths.at(2).push_back(iw::vector3(36 - 64, 1, 0));

				m_paths.at(2).push_back(iw::vector3(36, 1, 0));
			}

			Space->Query<RaftEnemy>().Each([&](auto e, auto) {
				Space->QueueEntity(e, iw::func_Destroy);
			});

			m_currentLevel = event.Level;

			for (int i = 0; i < m_paths.size(); i++) {
				for (int j = 0; j < (int)m_paths.at(i).size() - 1; j += 2) {
					Bus->push<SpawnEnemyEvent>(m_spin, m_paths.at(i).at(j), 0, m_currentLevel);
				}
			}

			break;
		}
		case iw::val(Actions::SPAWN_ENEMY): {
			SpawnEnemyEvent& event = e.as<SpawnEnemyEvent>();
			if (!event.SpawnedEnemy) {
				LOG_WARNING << "The raft enemy system needs to run after the enemy system!";
				return false;
			} 

			for (unsigned i = 0; i < m_paths.size(); i++) {
				auto itr = std::find(m_paths.at(i).begin(), m_paths.at(i).end(), event.Position);

				if (itr == m_paths.at(i).end()) { // this is bad but works for now
					continue; // exit if not spawned on a path node, aka not a raft enemy
				}

				iw::CollisionObject* c = GetPhysicsComponent(event.SpawnedEnemy.Handle);
				iw::Transform*       t = event.SpawnedEnemy.Find<iw::Transform>();
				
				iw::Transform* p = t->Parent();

				t->SetParent(nullptr);
				
				Physics->RemoveCollisionObject(c); // make this happen automatically, add event for event changed in Space

				RaftEnemy* r = event.SpawnedEnemy.Add<RaftEnemy>();

				r->PathIndex = i;
				r->IntraPathIndex = std::distance(m_paths.at(i).begin(), itr);

				c = GetPhysicsComponent(event.SpawnedEnemy.Handle);
				t = event.SpawnedEnemy.Find<iw::Transform>();

				t->SetParent(p);

				c->SetCol(event.SpawnedEnemy.Find<iw::SphereCollider>());
				c->SetTrans(t);
				
				Physics->AddCollisionObject(c);
			}

			break;
		}
	}

	return false;
}
