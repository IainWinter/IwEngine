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
	float m_sx, m_sy;  // Cells per meter
	float m_deltaTime; // Gets set in SandLayer::UpdateSystems

public:
	SandWorldUpdateSystem(
		SandWorld* world
	)
		: SystemBase("Sand World Update")
		, m_world(world)
		, m_deltaTime(0.f)
	{
		SetCameraScale(1, 1);
	}

	void SetCameraScale(
		float sx, float sy)
	{
		m_sx = sx; m_sy = sy;
	}

	IW_PLUGIN_SAND_API void Update() override;
};

IW_PLUGIN_SAND_END
