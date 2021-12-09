#pragma once

#include "Layers/Menu_Layer.h"
#include "iw/util/to_string.h"


#include "Upgrade.h"
#include "Events.h"

#include <array>

struct Menu_Upgrade_Layer : Menu_Layer2
{
	std::array<UpgradeDescription*, 8 * 8> upgrades;
	std::vector<std::pair<int, UpgradeDescription*>> active;

	ImFont* font;

	UpgradeDescription* tooltip;
	int money;

	Menu_Upgrade_Layer(
		int totalMoney
	)
		: Menu_Layer2 ("Upgrade menu")
		, money       (totalMoney)
		, tooltip     (nullptr)
	{}

	int Initialize() override;;

	void UI() override;

	void UpgradeTable();
	void ActiveTable();
	void Tooltip();
	void Buttons();

//void Menu_PostGame_Layer::SubmitScoreAndExit(
//	const std::string& whereTo)
//{
//	Console->QueueCommand(whereTo);
//
//	//iw::HttpRequest<iw::None> request;
//	//request.Ip       = "71.233.150.182";
//	//request.Host     = "data.winter.dev";
//	//request.Resource = "/regolith/php/submit_highscore.php";
//
//	//request.SetArgument("name", m_playerName);
//	//request.SetArgument("score", to_string(m_finalScore));
//
//	//m_connection.Request<std::string>(request, [](
//	//	std::string& str) 
//	//{
//	//	LOG_INFO << str;
//	//});
//}
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
