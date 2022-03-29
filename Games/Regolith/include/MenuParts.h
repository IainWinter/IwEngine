#pragma once

#include "Layers/Menu_Layer.h"
#include "iw/util/io/Network.h"

#include "iw/util/reflection/serialization/JsonSerializer.h"

#include "Upgrade.h"
#include "Events.h"
#include "MenuState.h"
#include "reflected/HighscoreRecord_reflected.h"
#include "iw/reflected/vector.h"

#include <array>

struct MenuParts
{
	float window_w;
	float window_h;

	float bg_h;
	float bg_w;
	float bg_x;
	float bg_y;

	float padding_1;
	float padding_01;

	ImGuiWindowFlags commonFlags;
	ImGuiWindowFlags commonFlagsFocus;

	MenuParts()
		: window_w   (0.f) 
		, window_h   (0.f)
		, bg_h       (0.f)
		, bg_w       (0.f)
		, bg_x       (0.f)
		, bg_y       (0.f)
		, padding_1  (0.f)
		, padding_01 (0.f)
	{
		commonFlags = 
			  ImGuiWindowFlags_NoDecoration 
			| ImGuiWindowFlags_NoResize 
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus;

		commonFlagsFocus =
			/*  ImGuiWindowFlags_NoDecoration
			|*/
			  ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove;
	}

	void UpdateBG(Menu_Layer* layer)
	{
		bg_x = layer->bg_x;
		bg_y = layer->bg_y;
		bg_w = layer->bg_w;
		bg_h = layer->bg_h;

		commonFlags = layer->commonFlags;
		commonFlagsFocus = layer->commonFlagsFocus;

		padding_1  = bg_x * .1f;
		padding_01 = bg_x * .01f;
	}
};

struct Highscores_MenuParts : MenuParts
{
	iw::NetworkConnection connection;

	std::vector<HighscoreRecord> scores;
	std::string gameId;
	std::string* name;

	ImFont* font_regular;
	ImFont* font_title;
	bool scroll_table;

	bool loading;

	Highscores_MenuParts()
		: MenuParts    () 
		, name         (nullptr)
		, font_regular (iwFont("verdana_36"))
		, font_title   (iwFont("verdana_92"))
		, scroll_table (true)
		, loading      (false)
	{}

	void ScoreTable(float x, float y, float w, float h);

	void SubmitTempNameAndGetGameId(int score);
	void UpdateTempNameWithRealName();

	void LoadTopScore();
	void LoadMoreScoresAbove();
	void LoadMoreScoresBelow();
};

struct Upgrade_MenuParts : MenuParts
{
	std::array<UpgradeDescription*, 7 * 4> upgrades;
	std::vector<std::pair<int, UpgradeDescription*>> active;

	int money;

	UpgradeDescription* tooltip;
	ImFont* font;
	ImFont* font_regular;
	ImFont* font_paragraph;

	Upgrade_MenuParts()
		: MenuParts      ()
		, money          (0)
		, tooltip        (nullptr)
		, font           (nullptr)
		, font_regular   (iwFont("verdana_36"))
		, font_paragraph (iwFont("verdana_18"))
	{
		RegisterImage("x.png");

		upgrades = {
			MakeThrusterBreaking(),     nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr,
			MakeThrusterAcceleration(), nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr,
			MakeThrusterMaxSpeed(),     MakeHealthPickupEfficiency(), MakeLaserPickupEfficiency(), nullptr, nullptr, nullptr, nullptr,
			nullptr,                    nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr,
		};

		//for (int i = 0; i < upgrades.size(); i++)
		//{
		//	upgrades[i] = MakeThrusterMaxSpeed();
		//}

		for (UpgradeDescription* description : upgrades)
		{
			if (!description) continue;
			RegisterImage(description->TexturePath);
		}
	}

	void UpgradeTable(float x, float y, float w, float h);
	void ActiveTable(float x, float y, float w, float h);
	void Tooltip(float x, float y);
};


struct Decals_MenuParts : MenuParts
{
	struct Line
	{
		float x, y, x2, y2;
		float width;
		ImVec4 color;
	};

	std::vector<Line> lines;

	void DrawLines()
	{
		for (const Line& line : lines)
		{
			ImGui::GetForegroundDrawList()->AddLine(
				ImVec2(bg_x + line.x, bg_y + line.y),
				ImVec2(bg_x + line.x2, bg_y + line.y2),
				ImGui::ColorConvertFloat4ToU32(line.color),
				line.width);
		}
	}
};

inline void RightAlign(const std::string& str)
{
	ImGui::SetCursorPosX(
		  ImGui::GetCursorPosX()
		+ ImGui::GetColumnWidth()
		- ImGui::CalcTextSize(str.c_str()).x
		- ImGui::GetScrollX()
		- ImGui::GetStyle().ItemSpacing.x * 2);
}
