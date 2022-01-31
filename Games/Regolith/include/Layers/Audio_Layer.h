#pragma once

#include "iw/engine/Layer.h"
#include "Events.h"

#undef CreateEvent

class Audio_Layer : public iw::Layer
{
private:
	int m_song; // temp handle to the song playing
	iw::Timer m_timer;

public:
	Audio_Layer()
		: iw::Layer ("Audio")
		, m_song    (-1)
	{}

	int Initialize() override;

	void Update() override
	{
		m_timer.Tick();
	}

	bool On(iw::ActionEvent& e) override;
};
