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
		for (WorldCoord i = 0; i < m_chunk.m_width * m_chunk.m_height; i++) {
			Cell& cell = m_chunk.m_cells[i];
			if (m_world.m_currentTick <= cell.LastUpdateTick) continue;

			WorldCoord x = i % m_chunk.m_width  + m_chunk.m_x;
			WorldCoord y = i / m_chunk.m_height + m_chunk.m_y;
			UpdateCell(x, y, cell);
		}
	}

	virtual void UpdateCell(
		WorldCoord x,
		WorldCoord y,
		Cell& cell) = 0;
protected:
	bool InBounds(
		WorldCoord x, WorldCoord y)
	{
		return  m_chunk.InBounds(x, y)
			|| m_world.InBounds(x, y);
	}

	bool IsEmpty(
		WorldCoord x, WorldCoord y)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.IsEmpty(x, y);
		}

		return m_world.IsEmpty(x, y);
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
