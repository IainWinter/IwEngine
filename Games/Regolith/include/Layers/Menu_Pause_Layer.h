#pragma once

#include "iw/engine/Layer.h"
#include "Assets.h"
#include "UI.h"

struct Menu_Pause_Layer : iw::Layer
{
	iw::Entity m_entity_screen;

	UI_Screen* m_screen;
	UI* m_pause_menu;
	UI* m_pause_title;
	UI_Button* m_button_test1;
	UI_Button* m_button_test2;
	UI_Button* m_button_test3;
	UI_Button* m_button_test4;

	iw::HandlerFunc* m_handle;

	float m_buttonOffset;
	bool m_execute;
	bool m_last_execute;
	//bool m_keyTyped;
	//char m_keyTypedChar;

	Menu_Pause_Layer()
		: iw::Layer      ("Menu pause")

		, m_screen       (nullptr)
		, m_pause_menu   (nullptr)
		, m_pause_title  (nullptr)
		, m_button_test1 (nullptr)
		, m_button_test2 (nullptr)
		, m_button_test3 (nullptr)
		, m_button_test4 (nullptr)

		, m_handle       (nullptr)

		, m_buttonOffset (0.f)
		, m_execute      (false)
		, m_last_execute (false)
	{}

	int Initialize() override;
	void Destroy() override;

	void OnPush() override;
	void OnPop() override;

	void PostUpdate() override;
};
