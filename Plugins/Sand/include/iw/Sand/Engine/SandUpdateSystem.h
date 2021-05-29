#pragma once

#include "iw/engine/System.h"
#include "../SandWorld.h"
#include "../Tile.h"

IW_PLUGIN_SAND_BEGIN

class SandWorldUpdateSystem
	: public SystemBase
{
private:
	SandWorld* m_world;

public:
	SandWorldUpdateSystem(
		SandWorld* world
	)
		: SystemBase("Sand World Update")
		, m_world(world)
	{}

	IW_PLUGIN_SAND_API void Update() override;
};

IW_PLUGIN_SAND_END
