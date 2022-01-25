#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "Components/CorePixels.h"
#include "Components/Asteroid.h"
#include "Components/Throwable.h"
#include "Events.h"

struct TileSplitSystem : iw::SystemBase
{
	iw::SandLayer* sand;
	iw::blocking_queue<size_t> splits;
	//std::thread splitThread;
	//bool splitThreadRunning;

	TileSplitSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase     ("Player")
		, sand               (sand)
		//, splitThreadRunning (false)
	{}

	//void OnPush() override;
	//void OnPop()  override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;

	void SplitTile(
		iw::Entity entity);

	void SplitTileOff(
		iw::Entity entity,
		std::vector<int> toSplit);

	void ExplodeTile(
		iw::Entity entity,
		std::vector<int> toSplit);
};
