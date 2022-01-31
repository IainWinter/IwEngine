#pragma once

#include "Layers/Menu_Layer.h"
#include "iw/util/to_string.h"

#include "Upgrade.h"
#include "Events.h"
#include "MenuState.h"

#include <vector>
#include <array>

struct Menu_Upgrade_Layer : Menu_Layer
{
	std::array<UpgradeDescription*, 8 * 8> upgrades;
	std::vector<std::pair<int, UpgradeDescription*>> active;

	int money;

	UpgradeDescription* tooltip;
	ImFont* font;
	ImFont* font_regular;
	ImFont* font_paragraph;

	Menu_Upgrade_Layer(
		int totalMoney
	)
		: Menu_Layer    ("Upgrade menu")
		, money          (totalMoney)
		, tooltip        (nullptr)
		, font           (nullptr)
		, font_regular   (nullptr)
		, font_paragraph (nullptr)
	{}

	int Initialize() override;

	void UI() override;

	void UpgradeTable();
	void ActiveTable();
	void Tooltip();
	void Buttons();
	void UpgradePreview();

	// speed upgrade preview
	std::vector<float> vals;
	Player player;
	void UpdateSpeedGraph();
};
