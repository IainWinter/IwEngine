#pragma once

#include "iw/engine/System.h"

class RiverBarrierSystem
	: public iw::SystemBase
{
private:
	std::vector<iw::Entity> m_barriers;
	iw::Entity& m_player;
	iw::Entity m_halfway;

public:
	RiverBarrierSystem(
		iw::Entity& player);

	void Update() override;

	bool On(iw::ActionEvent& e) override;
};
