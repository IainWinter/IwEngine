#pragma once

#include "Layers/Menu_Layer.h"
#include "iw/util/io/Network.h"

#include "iw/util/reflection/serialization/JsonSerializer.h"

#include "Upgrade.h"
#include "Events.h"
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
			  ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove;
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

	Highscores_MenuParts()
		: name         (nullptr)
		, font_regular (iwFont("verdana_36"))
		, font_title   (iwFont("verdana_92"))
		, scroll_table (true)
	{}

	void ScoreTable(float x, float y, float w, float h);

	void SubmitTempNameAndGetGameId(int score);
	void UpdateTempNameWithRealName();

	void LoadTopScore(
		std::function<void()> then);
	void LoadMoreScoresAbove();
	void LoadMoreScoresBelow();
};

struct Menu_Highscores_Layer : Menu_Layer2
{
	int score;

	Highscores_MenuParts highscoreParts;

	Menu_Highscores_Layer(
		int score
	)
		: Menu_Layer2 ("Main title menu")
		, score       (score)
	{}

	int Initialize() override;

	void UI() override;

	void PlayerImage();
	void Title();
	void Buttons();
};
