#pragma once

#include "Layers/Menu_Layer.h"

struct Menu_Fadeout_Layer : Menu_Layer2
{
	float m_time;
	float m_timer;
	float m_delay;

	Menu_Fadeout_Layer(
		float time,
		float delay
	)
		: Menu_Layer2   ("Menu fadeout game")

		, m_time        (time)
		, m_delay       (delay)
		, m_timer       (0.f)
	{}

	void UI() override;
};
