#pragma once

#include "iw/engine/System.h"
#include "Upgrade.h"
#include "Events.h"

struct Upgrade_System : iw::SystemBase
{
	UpgradeSet currentSet;

	Upgrade_System()
		: iw::SystemBase("Upgrade")
	{}

	void Update() override;
	bool On(iw::ActionEvent& e) override;
};
