#pragma once

#include "iw/engine/Layer.h"
#include "Assets.h"
#include "UI.h"

struct Menu_PostGame_Layer : iw::Layer
{
	UI_Screen* m_screen;

	UI* m_background;
	UI* m_pause_menu;
	UI* m_pause_title;

	Menu_PostGame_Layer()
		: iw::Layer     ("Menu post game")

		, m_screen      (nullptr)
		, m_background  (nullptr)
		, m_pause_menu  (nullptr)
		, m_pause_title (nullptr)
	{}

	int Initialize() override;
	void Destroy() override;
	void PostUpdate() override;
};
