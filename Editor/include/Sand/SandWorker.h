#pragma once

#include "SandWorld.h"

class SandWorker {
private:
	SandWorld& m_world;
	SandChunk& m_chunk;

public:
	SandWorker(
		SandWorld& world,
		SandChunk& chunk)
		: m_world(world)
		, m_chunk(chunk)
	{}

	void UpdateChunk() {
		for(WorldCoord x = 0/*m_chunk.m_minX*/; x < m_chunk.m_width /*m_chunk.m_maxX*/; x++)
		for(WorldCoord y = 0/*m_chunk.m_minY*/; y < m_chunk.m_height/*m_chunk.m_maxY*/; y++) {
			Cell& cell = m_chunk.GetCellDirect(x, y);
			if (m_world.m_currentTick <= cell.LastUpdateTick) continue;

			UpdateCell(x + m_chunk.m_x, y + m_chunk.m_y, cell);
		}
	}

	virtual void UpdateCell(
		WorldCoord x,
		WorldCoord y,
		Cell& cell) = 0;

	virtual void UpdateHitCell(
		WorldCoord hx, WorldCoord hy,
		WorldCoord x,  WorldCoord y,
		Cell& cell) {}
protected:
	Tick CurrentTick() const {
		return m_world.m_currentTick;
	}

	int Width() const {
		return m_chunk.m_width; //u -> s
	}

	int Height() const {
		return m_chunk.m_height;
	}

	bool InBounds(
		WorldCoord x, WorldCoord y)
	{
		return  m_chunk.InBounds(x, y)
			|| m_world.InBounds(x, y);
	}

	bool HasPrecedence(
		WorldCoord x,  WorldCoord y,
		WorldCoord tx, WorldCoord ty)
	{
		if (!InBounds(x, y) || !InBounds(tx, ty)) {
			return false;
		}

		return GetCell(x, y).Precedence > GetCell(tx, ty).Precedence;
	}

	bool IsEmpty(
		WorldCoord x, WorldCoord y)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.IsEmpty(x, y);
		}

		return m_world.IsEmpty(x, y);
	}

	void KeepAlive(
		WorldCoord x, WorldCoord y)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.KeepAlive(x, y);
		}
	}

	const Cell& GetCell(
		WorldCoord x, WorldCoord y)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.GetCell(x, y);
		}

		return m_world.GetCell(x, y);
	}

	void SetCell(
		WorldCoord x, WorldCoord y,
		const Cell& cell)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.SetCell(x, y, cell, m_world.m_currentTick);
		}

		return m_world.SetCell(x, y, cell);
	}

	void SetCellQueued(
		WorldCoord x, WorldCoord y,
		const Cell& cell)
	{
		if (m_chunk.InBounds(x, y)) {
			m_chunk.SetCellQueued(x, y, cell);
		}

		else {
			m_world.SetCellQueued(x, y, cell);
		}
	}

	void MoveCell(
		WorldCoord x,   WorldCoord y,
		WorldCoord xTo, WorldCoord yTo)
	{
		if (    m_chunk.InBounds(x, y)
			&& m_chunk.InBounds(xTo, yTo))
		{
			m_chunk.MoveCell(x, y, xTo, yTo);
		}

		else if (m_chunk.InBounds(x, y)) {
			m_world.MoveCell(x, y, xTo, yTo);
		}

		else {
			m_chunk.MoveCell(m_world.GetChunk(x, y), x, y, xTo, yTo);
		}
	}
};

// Helper functions

inline std::vector<WorldCoords> FillLine(
	int x,  int y,
	int x2, int y2)
{
	std::vector<WorldCoords> positions; // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

	int dx =  abs(x2 - x);
	int dy = -abs(y2 - y);
	int sx = x < x2 ? 1 : -1;
	int sy = y < y2 ? 1 : -1;
	int err = dx + dy;  /* error value e_xy */
		
	while (true) {
		positions.emplace_back(x, y);

		if (x == x2 && y == y2) break;
			
		int e2 = 2 * err;
		if (e2 >= dy) { /* e_xy + e_x > 0 */
			err += dy;
			x += sx;
		}

		if (e2 <= dx) { /* e_xy + e_y < 0 */
			err += dx;
			y += sy;
		}
	}

	return positions;
}
