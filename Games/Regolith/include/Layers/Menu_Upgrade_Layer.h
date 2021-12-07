#pragma once

#include "Layers/Menu_Layer.h"
#include "Assets.h"
#include "Upgrade.h"
#include "Events.h"

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
	Bought_Table*  m_bought;

	UI_Text* m_text_money;
	int m_money;

	UI_Button* m_button_reform;

	std::vector<UpgradeDescription*> toActivate;

	Menu_Upgrade_Layer(
		int finalScore
	)
		: Menu_Layer      ("Upgrade menu")
					   
		, m_money         (4000/*finalScore*/)
		, m_background    (nullptr)
		, m_tooltip       (nullptr)
		, m_upgrades      (nullptr)
		, m_bought        (nullptr)
		, m_text_money    (nullptr)
		, m_button_reform (nullptr)
	{}

	UI_Base* AddUpgrade(
		int x, int y,
		UpgradeDescription* upgrade);

	void UpdateMoneyText(
		int diff);

	int Initialize() override;
	void PostUpdate() override;
};
