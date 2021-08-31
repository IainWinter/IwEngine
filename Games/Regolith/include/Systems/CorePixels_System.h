#pragma once

#include "iw/engine/System.h"
#include "Components/CorePixels.h"
#include "plugins/iw/Sand/Tile.h"

#include "Events.h"

struct CorePixelsSystem : iw::SystemBase
{
	CorePixelsSystem()
		: iw::SystemBase("Core pixels")
	{}

	void Update();

	bool On(iw::EntityCreatedEvent& e) override;
	bool On(iw::ActionEvent& e)        override;
};
