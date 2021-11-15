#pragma once

#include "Layers/Menu_Layer.h"

struct Menu_Pause_Layer : Menu_Layer
{
	UI* m_pause_menu;
	UI* m_pause_title;
	UI* m_volume_title;

	UI_Button* m_button_resume;
	UI_Slider* m_slider_music;
	UI_Slider* m_slider_effect;

	std::vector<UI*> m_items;

	Menu_Pause_Layer()
		: Menu_Layer      ("Menu pause")

		, m_pause_menu    (nullptr)
		, m_pause_title   (nullptr)
		, m_slider_music  (nullptr)
		, m_slider_effect (nullptr)
	{}

	int Initialize() override;
	void PostUpdate() override;
};
