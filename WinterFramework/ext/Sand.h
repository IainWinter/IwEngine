#pragma once

#include <vector>
#include <functional>
#include "../Rendering.h"
#include "../Entity.h"

enum class CellMovement
{
	MOVE_FORWARD
};

struct Cell
{
	float x, y;
	Color color;
};

struct SandWorld
{
	entity display;

	// Going to store each particle in a list instead of a fixed grid
	std::vector<Cell> cells;

	SandWorld() = default;

	SandWorld(int w, int h, int camScaleX, int camScaleY)
	{
		display = entities().create()
			.add<Transform2D>(0, 0, 0, camScaleX, camScaleY, 0)
			.add<Texture>(w, h, 4, false);
	}

	~SandWorld()
	{
		// if not moved
		if (display.is_alive()) display.destroy();
	}

	entity CreateCell(float x, float y, Color color)
	{
		return entities().create().add<Cell>(x, y, color);
	}

	void Update()
	{
		Texture& disp = display.get<Texture>();

		disp.ClearHost();

		for (auto [_, cell] : entities().query<const Cell>())
		{
			int x = floor(cell.x);
			int y = floor(cell.y);
			if (x < 0 || y < 0 || x >= disp.Width() || y >= disp.Height()) continue;
			disp.Get(x, y) = cell.color;
		}

		disp.SendToDevice();
	}
};

#include "../ext/Time.h"

struct CellVel
{
	float x, y;
};

struct CellTime
{
	float life;
};

struct Sand_LifeUpdateSystem : For_System<CellTime>
{
	void Update(entity e, CellTime& time) override
	{
		time.life -= Time::DeltaTime();
		if (time.life <= 0.f)
		{
			e.destroy();
		}
	}
};

struct Sand_VelUpdateSystem : For_System<Cell, CellVel>
{
	void Update(entity e, Cell& cell, CellVel& vel) override
	{
		cell.x += vel.x;
		cell.y += vel.y;
	}
};