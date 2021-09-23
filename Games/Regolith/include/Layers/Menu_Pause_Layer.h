#pragma once

#include "iw/engine/Layer.h"
#include "Assets.h"
#include "UI.h"

struct Menu_Pause_Layer : iw::Layer
{
	iw::Entity m_entity_screen;

	UIScreen* m_screen;

	UI* m_background;
	UI* m_pause_menu;
	UI* m_pause_title;

	Menu_Pause_Layer()
		: iw::Layer     ("Menu pause")

		, m_screen      (nullptr)
		, m_background  (nullptr)
		, m_pause_menu  (nullptr)
		, m_pause_title (nullptr)
	{}

	int Initialize() override;
	void Destroy() override;

	void OnPush() override;
	void OnPop() override;

	void PostUpdate() override;
};
