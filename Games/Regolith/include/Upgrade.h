#pragma once

#include "iw/engine/AppVars.h"
#include "Components/Player.h"
#include <functional>
#include <string>
#include <vector>

struct UpgradeDescription;

struct Upgrade
{
	UpgradeDescription* Description;
	std::function<bool(iw::AppVars, float)> Action;

	inline bool CallAction(iw::AppVars app);
};

struct UpgradeDescription
{
	std::string Name;
	std::string Description;
	std::string TexturePath;

	// cost and stat # for each number bought
	// this is required to move up and down non linear increases
	std::vector<std::pair<int, int>> CostStatTable;

	int NumberBought = 0;

	Upgrade Upgrade;

	UpgradeDescription()
	{
		Upgrade.Description = this;
	}
};

inline bool Upgrade::CallAction(iw::AppVars app)
{
	if (!Action) return false;
	return Action(app, Description->CostStatTable[Description->NumberBought - 1].second);
}

struct UpgradeSet
{
	std::vector<Upgrade> Upgrades;
	std::vector<int> Active; // index of active upgrades

	void AddUpgrade(
		const Upgrade& upgrade)
	{
		Active  .push_back(Upgrades.size());
		Upgrades.push_back(upgrade);
	}
};

inline Player* GetPlayer(iw::AppVars& app)
{
	Player* p = nullptr;

	app.Space->Query<Player>().First(
		[&p](
			iw::EntityHandle handle,
			Player* player)
		{
			p = player;
		}
	);

	return p;
}

inline UpgradeDescription* MakeThrusterMaxSpeed()
{
	UpgradeDescription* description = new UpgradeDescription();

	description->Name        = "Thruster Velocity";
	description->Description = "Effects how fast the\nship can move";
	description->TexturePath = "upgrade_thrustermaxspeed.png";

	int cost = 2000;
	int stat = 10;

	for (int i = 0; i < 10; i++)
	{
		description->CostStatTable.emplace_back(cost, stat);
		cost *= 1.1;
		stat *= 1.5;
	}

	description->Upgrade.Action = [](iw::AppVars app, float stat)
	{
		GetPlayer(app)->u_speed += stat;
		return true;
	};

	return description;
}

inline UpgradeDescription* MakeThrusterBreaking()
{
	UpgradeDescription* description = new UpgradeDescription();

	description->Name        = "Thruster Breaking";
	description->Description = "Decrease the breaking force by <c>10 m/s/s</>";
	description->TexturePath = "upgrade_thrusterbreaking.png";

	int cost = 3000;
	int stat = 50;

	for (int i = 0; i < 10; i++)
	{
		description->CostStatTable.emplace_back(cost, stat);
		cost *= 1.1;
		stat *= 1.5;
	}

	description->Upgrade.Action = [](iw::AppVars app, float stat)
	{
		GetPlayer(app)->u_breaking += stat;
		return true;
	};

	return description;
}

inline UpgradeDescription* MakeThrusterAcceleration()
{
	UpgradeDescription* description = new UpgradeDescription();

	description->Name        = "Thruster Acceleration";
	description->Description = "Increase the thruster force.";
	description->TexturePath = "upgrade_thrusteracceleration.png";

	int cost = 3000;
	int stat = 50;

	for (int i = 0; i < 10; i++)
	{
		description->CostStatTable.emplace_back(cost, stat);
		cost *= 1.1;
		stat *= 1.5;
	}

	description->Upgrade.Action = [](iw::AppVars app, float stat)
	{
		GetPlayer(app)->u_acceleration += stat;
		return true;
	};

	return description;
}

inline UpgradeDescription* MakeHealthPickupEfficiency()
{
	UpgradeDescription* description = new UpgradeDescription();

	description->Name        = "Health Pickup Efficiency";
	description->Description = "Regenerate an additional <c>2 cells</> per health pickup.";
	description->TexturePath = "upgrade_healthefficiency.png";
	description->CostStatTable.emplace_back(2000, 10);

	description->Upgrade.Action = [](iw::AppVars app, float stat)
	{
		GetPlayer(app)->u_speed += 100;
		return true;
	};

	return description;
}

inline UpgradeDescription* MakeLaserPickupEfficiency()
{
	UpgradeDescription* description = new UpgradeDescription();

	description->Name        = "Laser Pickup Efficiency";
	description->Description = "Effects how much liquid get\nconverted per red pickup";
	description->TexturePath = "upgrade_laserefficiency.png";
	description->CostStatTable.emplace_back(3000, 10);

	description->Upgrade.Action = [](iw::AppVars app, float stat)
	{
		GetPlayer(app)->u_speed += 100;
		return true;
	};

	return description;
}
