#pragma once

#include "Layers/Menu_Layer.h"

struct Menu_Main_Layer : Menu_Layer2
{
	Menu_Main_Layer()
		: Menu_Layer2("Main menu")
	{}

	int Initialize() override;
	void UI() override;
};
