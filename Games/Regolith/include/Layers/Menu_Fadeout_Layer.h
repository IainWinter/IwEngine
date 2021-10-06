#pragma once

#include "Layers/Menu_Layer.h"

struct Menu_Fadeout_Layer : Menu_Layer
{
	UI* m_background;

	float m_time;
	float m_timer;

	Menu_Fadeout_Layer(
		float time
	)
		: Menu_Layer    ("Menu fadeout game")

		, m_background  (nullptr)

		, m_time        (time)
		, m_timer       (0.f)
	{}

	int Initialize() override;
	void PostUpdate() override;
};
