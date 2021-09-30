#pragma once

#include "iw/engine/Layer.h"
#include "Assets.h"
#include "UI.h"

struct Menu_PostGame_Layer : iw::Layer
{
	UI_Screen* m_screen;

	UI* m_background;
	UI* m_title;
	UI* m_score_title;
	UI* m_score;
	UI* m_player;
	UI_Button* m_restart;

	iw::HandlerFunc* m_handle;
	bool m_execute;
	bool m_last_execute;

	Menu_PostGame_Layer()
		: iw::Layer      ("Menu post game")
					  
		, m_screen       (nullptr)
		, m_background   (nullptr)
		, m_title        (nullptr)
		, m_score_title  (nullptr)
		, m_score        (nullptr)
		, m_player       (nullptr)
		, m_restart      (nullptr)
					  
		, m_handle       (nullptr)
		, m_execute      (false)
		, m_last_execute (false)
	{}

	int Initialize() override;
	void Destroy() override;
	void PostUpdate() override;
};