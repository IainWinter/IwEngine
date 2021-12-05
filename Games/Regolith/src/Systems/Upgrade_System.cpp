#pragma once

#include "Systems/Upgrade_System.h"

void Upgrade_System::Update()
{
	for (int index : currentSet.Active)
	{
		Upgrade& upgrade = currentSet.Upgrades[index];

		if (  !upgrade.Action 
			|| upgrade.Action(APP_VARS_LIST))
		{
			LOG_INFO << "Applying upgrade " << upgrade.Name;

			currentSet.Upgrades.erase(
				currentSet.Upgrades.begin() + index
			);
		}
	}
}

bool Upgrade_System::On(iw::ActionEvent& e)
{
	switch (e.Action)
	{
		case APPLY_UPGRADE_SET:
		{
			currentSet = e.as<ApplyUpgradeSet_Event>().Set;
			break;
		}
	}

	return false;
}
