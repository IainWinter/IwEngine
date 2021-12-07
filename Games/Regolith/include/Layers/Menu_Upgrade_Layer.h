#pragma once

#include "iw/engine/Layers/ImGuiLayer.h"
#include "Upgrade.h"
#include "Events.h"
#include <unordered_map>
#include <array>
#include "iw/util/to_string.h"

struct Menu_Upgrade_Layer : iw::Layer
{
	std::unordered_map<std::string, void*> imgs;
	std::array<UpgradeDescription*, 8 * 8> upgrades;

	std::vector<std::pair<int, UpgradeDescription*>> active;

	float window_w;
	float window_h;
	
	float bg_h;
	float bg_w;
	float bg_x;
	float bg_y;

	ImGuiWindowFlags commonFlags;

	UpgradeDescription* tooltip;
	int money;

	Menu_Upgrade_Layer(
		int totalMoney
	)
		: iw::Layer ("Upgrade menu")
		, money     (totalMoney)
		, tooltip   (nullptr)
	{}

	int Initialize() override;;
	void ImGui() override;

	void Background();
	void UpgradeTable();
	void ActiveTable();
	void Tooltip();

	void Buttons();

	void RegisterImage(const std::string& str);
};


//#pragma once
//
//#include "Layers/Menu_Layer.h"
//#include "Assets.h"
//#include "Upgrade.h"
//#include "Events.h"
//
//struct Menu_Upgrade_Layer : Menu_Layer
//{
//	using Upgrade_Table = UI_Table<
//		UpgradeDescription*,
//		UpgradeDescription*,
//		UpgradeDescription*,
//		UpgradeDescription*,
//		UpgradeDescription*
//	>;
//
//	using Bought_Table  = UI_Table<
//		UpgradeDescription*,
//		std::string
//	>;
//
//	UI* m_background;
//	UI* m_tooltip;
//
//	Upgrade_Table* m_upgrades;
//	Bought_Table*  m_bought;
//
//	UI_Text* m_text_money;
//	int m_money;
//
//	UI_Button* m_button_reform;
//
//	std::vector<UpgradeDescription*> toActivate;
//
//	Menu_Upgrade_Layer(
//		int finalScore
//	)
//		: Menu_Layer      ("Upgrade menu")
//					   
//		, m_money         (4000/*finalScore*/)
//		, m_background    (nullptr)
//		, m_tooltip       (nullptr)
//		, m_upgrades      (nullptr)
//		, m_bought        (nullptr)
//		, m_text_money    (nullptr)
//		, m_button_reform (nullptr)
//	{}
//
//	UI_Base* AddUpgrade(
//		int x, int y,
//		UpgradeDescription* upgrade);
//
//	void UpdateMoneyText(
//		int diff);
//
//	int Initialize() override;
//	void PostUpdate() override;
//};
