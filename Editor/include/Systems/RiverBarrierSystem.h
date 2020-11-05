#pragma once

#include "iw/engine/System.h"

class RiverBarrierSystem
	: public iw::SystemBase
{
private:
	std::vector<iw::Entity> m_barriers;

public:
	RiverBarrierSystem();

	bool On(iw::ActionEvent& e) override;
};
