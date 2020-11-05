#pragma once

#include "iw/engine/System.h"

class RiverRaftEnemySystem
	: public iw::SystemBase
{
private:
	std::vector<iw::vector3> m_path;

	iw::Transform* m_currentLevel;
	bool m_running;

public:
	RiverRaftEnemySystem();

	int Initialize() override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;
};