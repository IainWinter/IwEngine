#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "Components/CorePixels.h"
#include "Components/Asteroid.h"
#include "Components/Throwable.h"
#include "Events.h"
#include "ECS.h"

struct TileSplitSystem : iw::SystemBase
{
	iw::SandLayer* sand;
	iw::blocking_queue<entity_handle> splits;
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
		entity entity);

	void SplitTileOff(
		entity entity,
		std::vector<int> toSplit);

	void ExplodeTile(
		entity entity,
		std::vector<int> toSplit);
};
