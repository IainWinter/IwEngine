#pragma once

#include "Layers/Menu_Layer.h"
#include "MenuState.h"

struct Menu_Bg_Render_Layer : Menu_Layer
{
	Menu_Bg_Render_Layer()
		: Menu_Layer ("Bg Render")
	{}

	void UI() override;
};
