#pragma once

#include "Layers/Menu_Layer.h"

#include "iw/reflected/vector.h"
#include "reflected/HighscoreRecord_reflected.h"

#include "iw/util/reflection/serialization/JsonSerializer.h"

struct Menu_PostGame_Layer : Menu_Layer
{
	UI* m_background;
	UI* m_playerBorder;
	UI* m_playerAnimate;
	UI* m_title_score;

	UI_Table* m_table_highScore;

	UI_Button* m_button_reform;
	UI_Button* m_button_quit;

	Menu_PostGame_Layer()
		: Menu_Layer       ("Menu post game")
					  
		, m_background     (nullptr)
		, m_playerBorder   (nullptr)
		, m_playerAnimate  (nullptr)
		, m_title_score    (nullptr)

		, m_table_highScore (nullptr)

		, m_button_reform  (nullptr)
		, m_button_quit    (nullptr)
	{}

	int Initialize() override;
	void PostUpdate() override;

	void LoadHighscores();
};
