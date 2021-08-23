#pragma once

#include "iw/engine/System.h"
#include "Events.h"

struct WeaponSystem : iw::SystemBase
{
	WeaponSystem()
		: iw::SystemBase("Weapon")
	{}

	void Update() override;

	bool On(iw::ActionEvent& e) override;
};
