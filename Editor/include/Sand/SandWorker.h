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

	virtual void UpdateChunk() = 0;
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
			return m_chunk.IsEmpty(m_world.GetChunkCoords(x, y));
		}

		return m_world.IsEmpty(x, y);
	}

	const Cell& GetCell(
		WorldCoord x, WorldCoord y)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.GetCell(m_world.GetChunkCoords(x, y));
		}

		return m_world.GetCell(x, y);
	}

	void SetCell(
		WorldCoord x, WorldCoord y,
		const Cell& cell)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.SetCell(m_world.GetChunkCoords(x, y), cell, m_world.m_currentTick);
		}

		return m_world.SetCell(x, y, cell);
	}
};
