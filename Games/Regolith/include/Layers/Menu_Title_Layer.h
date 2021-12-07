#pragma once

#include "iw/engine/Layers/ImGuiLayer.h"
#include "Upgrade.h"
#include "Events.h"
#include <unordered_map>
#include <array>

struct Menu_Title_Layer : iw::Layer
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

	Menu_Title_Layer()
		: iw::Layer ("Main title menu")
		, money     (3000)
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
