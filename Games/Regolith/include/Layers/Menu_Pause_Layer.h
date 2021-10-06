#pragma once

#include "Layers/Menu_Layer.h"

struct Menu_Pause_Layer : Menu_Layer
{
	UI* m_pause_menu;
	UI* m_pause_title;

	std::vector<UI_Button*> m_buttons;

	float m_buttonOffset;
	//bool m_keyTyped;
	//char m_keyTypedChar;

	void AddButton(const iw::Mesh& bg, const iw::Mesh& label)
	{
		m_buttons.push_back(m_screen->CreateElement<UI_Button>(bg, label));
	}

	Menu_Pause_Layer()
		: Menu_Layer    ("Menu pause")

		, m_pause_menu   (nullptr)
		, m_pause_title  (nullptr)

		, m_buttonOffset (0.f)
	{}

	int Initialize() override;
	void PostUpdate() override;
};
