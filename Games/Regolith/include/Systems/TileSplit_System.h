#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "Components/CorePixels.h"
#include "Events.h"
#include <unordered_set>

struct TileSplitSystem : iw::SystemBase
{
	iw::SandLayer* sand;

	TileSplitSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase ("Player")
		, sand           (sand)
	{}

	bool On(iw::ActionEvent& e) override;

	void SplitTileOff(
		iw::Entity entity,
		std::vector<int> toSplit);
};
