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

struct CellVel
{
	float x, y, dampen = 20;
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

		//display.get<Texture>().InitAsRenderTarget();
	}

	~SandWorld()
	{
		display.destroy();
	}

	entity CreateCell(float x, float y, Color color)
	{
		return entities().create().add<Cell>(x, y, color);
	}

	void Update()
	{
		Texture& disp = display.get<Texture>();

		disp.Clear();

		for (auto [_, cell, vel] : entities().query<Cell, CellVel>())
		{
			//DrawPixel(disp, cell.x, cell.y, cell.color);
			DrawLine(disp, cell.x, cell.y, cell.x + vel.x, cell.y + vel.y, vel.dampen, cell.color);
		}

		disp.SendToDevice();
	}

private:
	void DrawLine(Texture& disp, float x0, float y0, float x1, float y1, float dampen, Color c)
	{
		float dx = x1 - x0;
		float dy = y1 - y0;
		dx /= dampen;
		dy /= dampen;

		float distance = sqrt(dx*dx + dy*dy);
		dx /= distance;
		dy /= distance;

		float x = x0;
		float y = y0;

		for (int i = 0; i < ceil(distance); i++)
		{
			DrawPixel(disp, x, y, c);
			x += dx;
			y += dy;
		}
	}

	void DrawPixel(Texture& disp, float xf, float yf, Color c)
	{
		int x = floor(xf) + disp.Width()  / 2;
		int y = floor(yf) + disp.Height() / 2;
		if (x > 0 && y > 0 && x < disp.Width() & y < disp.Height())
		{
			disp.Get(x, y) = c;
		}
	}
};

#include "../ext/Time.h"

struct CellLife
{
	float life;
};

struct Sand_LifeUpdateSystem : For_System<CellLife>
{
	void Update(entity e, CellLife& time) override
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
		cell.x += vel.x * Time::DeltaTime();
		cell.y += vel.y * Time::DeltaTime();
	}
};