#pragma once

#include "Layers/Menu_Layer.h"

#include "iw/reflected/vector.h"
#include "reflected/HighscoreRecord_reflected.h"

#include "iw/util/io/Network.h"
#include "iw/util/reflection/serialization/JsonSerializer.h"

#include "iw/util/algorithm/guid.h"

struct Menu_PostGame_Layer : Menu_Layer
{
	UI* m_background;
	UI* m_playerBorder;
	UI* m_playerAnimate;
	UI* m_title_score;

	// for fade
	iw::ref<iw::Material> mat_tableItem;
	iw::ref<iw::Material> mat_tableText;
	iw::ref<iw::Material> mat_tablePlayerRow;

	using Score_Table = UI_Table<int, int, std::string>;

	Score_Table* m_table_highScore;

	UI_Button* m_button_reform;
	UI_Button* m_button_quit;

	iw::NetworkConnection m_connection;
	std::string m_gameId;

	int m_finalScore;
	int m_playerRowId;
	std::string m_playerName;

	Menu_PostGame_Layer(
		int finalScore
	)
		: Menu_Layer        ("Menu post game")
					     
		, m_finalScore      (finalScore)
		, m_playerRowId     (-1)
					     
		, m_background      (nullptr)
		, m_playerBorder    (nullptr)
		, m_playerAnimate   (nullptr)
		, m_title_score     (nullptr)

		, m_table_highScore (nullptr)
		, mat_tableItem     (nullptr)
		, mat_tableText     (nullptr)

		, m_button_reform   (nullptr)
		, m_button_quit     (nullptr)
	{}

	int Initialize() override;
	void PostUpdate() override;

	bool On(iw::KeyTypedEvent& e) override;
	bool On(iw::MouseWheelEvent& e) override;

	int AddHighscoreToTable(
		const HighscoreRecord& record,
		bool makeInstanceOfBackgroundMesh = false);

	iw::NetworkResult<std::string> SubmitTempScoreAndGetId();

	void SubmitScoreAndExit(
		const std::string& whereTo);

	void SetTableBoxFade(
		float minX, float minY,
		float maxX, float maxY);
};
