#pragma once

#include "iw/engine/System.h"
#include "Components/Enemy.h"

class RiverRaftEnemySystem
	: public iw::SystemBase
{
private:
	std::vector<std::vector<iw::vector3>> m_paths;

	iw::Transform* m_currentLevel;

	float m_start;
	Enemy  m_spin, m_circle;
	Bullet m_line, m_seek, m_orbit;

public:
	RiverRaftEnemySystem();

	int Initialize() override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;
};
