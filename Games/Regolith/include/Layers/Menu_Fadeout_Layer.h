#pragma once

#include "iw/engine/Layer.h"
#include "Assets.h"
#include "UI.h"

struct Menu_Fadeout_Layer : iw::Layer
{
	UI_Screen* m_screen;
	UI*        m_background;

	float m_time;
	float m_timer;

	Menu_Fadeout_Layer(
		float time
	)
		: iw::Layer     ("Menu fadeout game")

		, m_screen      (nullptr)
		, m_background  (nullptr)

		, m_time        (time)
		, m_timer       (0.f)
	{}

	int Initialize() override;
	void Destroy() override;
	void PostUpdate() override;
};
