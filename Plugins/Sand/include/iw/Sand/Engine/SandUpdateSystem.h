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

	int m_sx, m_sy; // Cells per meter

public:
	SandWorldUpdateSystem(
		SandWorld* world
	)
		: SystemBase("Sand World Update")
		, m_world(world)
	{
		SetCameraScale(1, 1);
	}

	void SetCameraScale(
		int sx, int sy)
	{
		m_sx = sx; m_sy = sy;
	}

	IW_PLUGIN_SAND_API void Update() override;
};

IW_PLUGIN_SAND_END
