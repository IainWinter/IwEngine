#pragma once

#include "Cell.h"
#include <vector>
#include <tuple>
#include <atomic>
#include <mutex>

struct SandChunk {
private:
	std::vector<std::tuple<SandChunk*, Index, Index>> m_changes; // source, destination
	std::mutex m_changesMutex;

public:
	Cell* m_cells; // doesn't own this memory

	const WorldCoord m_x;
	const WorldCoord m_y;
	const ChunkCoord m_width;
	const ChunkCoord m_height;

	WorldCoord m_minX, m_minY,
		      m_maxX, m_maxY; // rect around cells to update

	std::atomic<uint32_t> m_filledCellCount;

	Tick m_lastTick;

	SandChunk();

	SandChunk(
		Cell* cells,
		WorldCoord x,     WorldCoord y,
		ChunkCoord width, ChunkCoord height,
		Tick currentTick);

	SandChunk(const SandChunk& copy);

	bool IsAllEmpty() const {
		return m_filledCellCount == 0;
	}

	bool	InBounds(
		WorldCoord x, WorldCoord y) const
	{
		return  x >= m_x && x < m_x + m_width
			&& y >= m_y && y < m_y + m_height;
	}

	bool CellAlreadyUpdated(
		WorldCoord x, WorldCoord y) const
	{
		return GetCell(x, y).LastUpdateTick == m_lastTick;
	}

	bool IsEmpty(
		WorldCoord x, WorldCoord y) const
	{
		return GetCell(x, y).Type == CellType::EMPTY;
	}

	Index GetIndex(
		WorldCoord x, WorldCoord y) const
	{
		x -= m_x;
		y -= m_y;

		return x + y * m_width;
	}

	Index GetIndexDirect(
		ChunkCoord x, ChunkCoord y) const
	{
		return x + y * m_width;
	}

	const Cell& GetCellDirect(ChunkCoord x, ChunkCoord y) const { return m_cells[GetIndexDirect(x, y)]; }

	const Cell& GetCell(WorldCoord x, WorldCoord y) const { return m_cells[GetIndex(x, y)]; }
	      Cell& GetCell(WorldCoord x, WorldCoord y)       { return m_cells[GetIndex(x, y)]; }
		 Cell& GetCell(Index index)				    { return m_cells[index]; }

	void SetCell(
		WorldCoord x, WorldCoord y,
		const Cell& cell,
		Tick currentTick)
	{
		SetCell(GetIndex(x, y), cell , currentTick);
	}

	void SetCell(
		Index index,
		const Cell& cell,
		Tick currentTick);

	void MoveCell(
		WorldCoord x,   WorldCoord y,
		WorldCoord xTo, WorldCoord yTo)
	{
		MoveCell(this, x, y, xTo, yTo);
	}

	void MoveCell(
		SandChunk* source,
		WorldCoord x,   WorldCoord y,
		WorldCoord xTo, WorldCoord yTo);

	void CommitMovedCells(
		Tick currentTick);
private:
	void ResetRect();

	void UpdateRect(
		WorldCoord x, WorldCoord y);

	void SetCellData(
		Index index,
		const Cell& cell,
		Tick currentTick);
};
