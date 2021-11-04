#pragma once

#include "iw/engine/Layer.h"

#undef CreateEvent

class Audio_Layer : public iw::Layer
{
private:
	iw::HandlerFunc* m_console;

	int m_song;

public:
	Audio_Layer()
		: iw::Layer ("Audio")
		, m_console (nullptr)
		, m_song    (-1)
	{}

	void OnPush() override;
	void OnPop()  override;

	bool On(iw::ActionEvent& e) override;
};
