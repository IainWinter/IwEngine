#pragma once

#include "iw/engine/AppVars.h"
#include "Components/Player.h"
#include <functional>
#include <string>

struct UpgradeDescription;

struct Upgrade
{
	UpgradeDescription* Description;

	std::function<bool        (iw::AppVars)> Action;
	std::function<std::string (iw::AppVars)> GetInfo;
};

struct UpgradeDescription
{
	std::string Name;
	std::string DispName;
	std::string Description;

	std::string TexturePath;

	std::string Stat;
	int Cost;

	iw::Color StatColor;
	iw::Color CostColor;

	std::function<Upgrade()> Create;

	UpgradeDescription(
		bool posOrNegStat)
	{
		if (posOrNegStat)
		{
			StatColor = iw::Color::From255(85, 255, 0);
		}

		else
		{
			StatColor = iw::Color::From255(255, 85, 0);
		}

		CostColor = iw::Color::From255(255, 210, 0);
	}
};


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
	UpgradeDescription* description = new UpgradeDescription(true);

	description->Name        = "Thruster Velocity";
	description->DispName    = "Thruster\nVelocity";
	description->Description = "Effects how fast the\nship can move";
	description->TexturePath = "upgrade_thrustermaxspeed.png";
	description->Stat        = "+10%";
	description->Cost        = 2000;

	Upgrade upgrade;
	upgrade.Description = description;

	upgrade.Action = [](iw::AppVars app)
	{
		GetPlayer(app)->speed += 100;
		return true;
	};

	upgrade.GetInfo = [](iw::AppVars app)
	{
		std::stringstream ss;
		ss << "Player Speed " << GetPlayer(app)->speed;
		return ss.str();
	};

	description->Create = [upgrade]()
	{
		return upgrade;
	};

	return description;
}

inline UpgradeDescription* MakeThrusterImpulse()
{
	UpgradeDescription* description = new UpgradeDescription(true);

	description->Name        = "Thruster Impulse";
	description->DispName    = "Thruster\nImpulse";
	description->Description = "Effects the immediate stop\nand start time of\nthe ship";
	description->TexturePath = "upgrade_thrusterimpulse.png";
	description->Stat        = "+10%";
	description->Cost        = 1000;

	Upgrade upgrade;
	upgrade.Description = description;

	upgrade.Action = [](iw::AppVars app)
	{
		GetPlayer(app)->speed += 1;
		return true;
	};

	upgrade.GetInfo = [](iw::AppVars app)
	{
		std::stringstream ss;
		ss << "Player Speed " << GetPlayer(app)->speed;
		return ss.str();
	};

	description->Create = [upgrade]()
	{
		return upgrade;
	};

	return description;
}

inline UpgradeDescription* MakeThrusterAcceleration()
{
	UpgradeDescription* description = new UpgradeDescription(true);

	description->Name        = "Thruster Acceleration";
	description->DispName    = "Thruster\nAcceleration";
	description->Description = "Effects the time to\nreach max speed";
	description->TexturePath = "upgrade_thrusteracceleration.png";
	description->Stat        = "+10%";
	description->Cost        = 1200;

	Upgrade upgrade;
	upgrade.Description = description;

	upgrade.Action = [](iw::AppVars app)
	{
		GetPlayer(app)->speed += 1;
		return true;
	};

	upgrade.GetInfo = [](iw::AppVars app)
	{
		std::stringstream ss;
		ss << "Player Speed " << GetPlayer(app)->speed;
		return ss.str();
	};

	description->Create = [upgrade]()
	{
		return upgrade;
	};

	return description;
}

inline UpgradeDescription* MakeHealthPickupEfficiency()
{
	UpgradeDescription* description = new UpgradeDescription(true);

	description->Name        = "Health Pickup Efficiency";
	description->DispName    = "Health Pickup\nEfficiency";
	description->Description = "Effects how many cells get\nrestored per green pickup";
	description->TexturePath = "upgrade_healthefficiency.png";
	description->Stat        = "+2 cells";
	description->Cost        = 2000;

	Upgrade upgrade;
	upgrade.Description = description;

	upgrade.Action = [](iw::AppVars app)
	{
		GetPlayer(app)->speed += 1;
		return true;
	};

	upgrade.GetInfo = [](iw::AppVars app)
	{
		std::stringstream ss;
		ss << "Player Speed " << GetPlayer(app)->speed;
		return ss.str();
	};

	description->Create = [upgrade]()
	{
		return upgrade;
	};

	return description;
}

inline UpgradeDescription* MakeLaserPickupEfficiency()
{
	UpgradeDescription* description = new UpgradeDescription(true);

	description->Name        = "Laser Pickup Efficiency";
	description->DispName    = "Laser Pickup\nEfficiency";
	description->Description = "Effects how much liquid get\nconverted per red pickup";
	description->TexturePath = "upgrade_laserefficiency.png";
	description->Stat        = "+2 laser cells";
	description->Cost        = 3000;

	Upgrade upgrade;
	upgrade.Description = description;

	upgrade.Action = [](iw::AppVars app)
	{
		GetPlayer(app)->speed += 1;
		return true;
	};

	upgrade.GetInfo = [](iw::AppVars app)
	{
		std::stringstream ss;
		ss << "Player Speed " << GetPlayer(app)->speed;
		return ss.str();
	};

	description->Create = [upgrade]()
	{
		return upgrade;
	};

	return description;
}
