#pragma once

#include "Layers/Menu_Layer.h"

struct Menu_PostGame_Layer : Menu_Layer
{
	UI* m_background;
	UI* m_playerBorder;
	UI* m_playerAnimate;
	UI* m_title_finalScore;

	UI_Button* m_button_reform;
	UI_Button* m_button_quit;

	float m_title_FinalScoreWidth;

	Menu_PostGame_Layer()
		: Menu_Layer              ("Menu post game")
					  
		, m_background            (nullptr)
		, m_playerAnimate         (nullptr)
		, m_title_finalScore      (nullptr)
		, m_button_reform         (nullptr)
		, m_button_quit           (nullptr)

		, m_title_FinalScoreWidth (0.f)
	{}

	int Initialize() override;
	void PostUpdate() override;
};
