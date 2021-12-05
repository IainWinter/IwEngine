#pragma once

#include <functional>
#include <string>

struct Upgrade
{
	std::string Name;
	std::function<bool(iw::AppVars)> Action;
};

struct UpgradeSet
{
	std::vector<Upgrade> Upgrades;
	std::vector<int> Active;
};

inline Upgrade MakeThrusterMaxSpeed()
{
	Upgrade up;
	up.Name = "Thruster Max Speed";
	up.Action = [](
		iw::AppVars app)
	{
		app.Space->Query<Player>().First(
			[](
				iw::EntityHandle handle,
				Player* player)
			{
				player->speed = 20;
			}
		);

		return true;
	};

	// alls this does is increase the player Speed variable


	return up;
}
