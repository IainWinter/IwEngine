#pragma once

#include "Layers/Menu_Layer.h"
#include "iw/util/io/Network.h"
#include "iw/util/reflection/serialization/JsonSerializer.h"

#include "Upgrade.h"
#include "Events.h"
#include "HighscoreRecord.h"

#include <array>

struct Menu_Title_Layer : Menu_Layer2
{
	iw::NetworkConnection connection;
	std::vector<HighscoreRecord> scores;
	int score;

	std::string gameId;
	std::string* name;

	ImFont* font_regular;
	ImFont* font_title;
	bool scroll_table;

	Menu_Title_Layer(
		int score = 1009
	)
		: Menu_Layer2  ("Main title menu")
		, score        (score)
		, scroll_table (true)
	{}

	int Initialize() override;

	void UI() override;

	void PlayerImage();
	void Title();
	void ScoreTable();
	void Buttons();

	void SubmitTempScoreAndGetId();
	void SubmitScoreAndExit(const std::string& command);
};
