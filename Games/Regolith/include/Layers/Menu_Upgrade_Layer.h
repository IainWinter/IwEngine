#pragma once

#include "Layers/Menu_Layer.h"
#include "Assets.h"
#include "Upgrade.h"

struct Menu_Upgrade_Layer : Menu_Layer
{
	using Upgrade_Table = UI_Table<
		UpgradeDescription*,
		UpgradeDescription*,
		UpgradeDescription*,
		UpgradeDescription*,
		UpgradeDescription*
	>;

	using Bought_Table  = UI_Table<
		UpgradeDescription*,
		std::string
	>;

	UI* m_background;
	UI* m_tooltip;


	Upgrade_Table* m_upgrades;
	Bought_Table* m_bought;

	UI_Text* m_text_money;
	int m_money;

	std::vector<UpgradeDescription*> toActivate;

	Menu_Upgrade_Layer(
		int finalScore
	)
		: Menu_Layer   ("Upgrade menu")

		, m_money      (2000/*finalScore*/)
		, m_background (nullptr)
		, m_upgrades   (nullptr)
	{}

	UI_Base* AddUpgrade(
		int x, int y,
		UpgradeDescription* upgrade);

	void UpdateMoneyText(
		int diff);

	int Initialize() override;
	void PostUpdate() override;
};
