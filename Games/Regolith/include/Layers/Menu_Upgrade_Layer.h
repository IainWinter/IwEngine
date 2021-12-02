#pragma once

#include "Layers/Menu_Layer.h"
#include "Assets.h"

struct Menu_Upgrade_Layer : Menu_Layer
{
	using Upgrade_Table = UI_Table<int, int, int, int, int>;

	UI* m_background;
	UI* m_tooltip;

	Upgrade_Table* m_upgrades;

	int m_money;

	struct UpgradeDescription
	{
		std::string texturePath;
		std::string tooltip;
		std::string description;
		std::string stat;
		std::string cost;
		iw::Color statColor;
		iw::Color costColor;
	};

	Menu_Upgrade_Layer(
		int finalScore
	)
		: Menu_Layer   ("Upgrade menu")

		, m_money      (finalScore)
		, m_background (nullptr)
		, m_upgrades   (nullptr)
	{}

	UI_Base* MakeUpgrade(
		const UpgradeDescription& upgrade);

	int Initialize() override;
	void PostUpdate() override;
};
