#pragma once

#include "Systems/Upgrade_System.h"

void Upgrade_System::Update()
{
	for (int i = 0; i < currentSet.Active.size(); i++)
	{
		Upgrade& upgrade = currentSet.Upgrades[currentSet.Active[i]];

		if (upgrade.CallAction(APP_VARS_LIST))
		{
			LOG_INFO << "Applied upgrade " << upgrade.Description->Name;

			currentSet.Active.erase(
				currentSet.Active.begin() + i
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
