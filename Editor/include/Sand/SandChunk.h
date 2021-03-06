#pragma once

#include "Cell.h"
#include <vector>
#include <tuple>
#include <atomic>
#include <mutex>

struct SandChunk {
private:
	std::vector<std::tuple<Index, Cell>> m_setQueue;
	std::vector<std::tuple<SandChunk*, Index, Index>> m_moveQueue; // source, destination
	std::vector<Index> m_keepAlive;
	std::mutex m_setMutex;
	std::mutex m_setChangesMutex;
	std::mutex m_changesMutex;
	std::mutex m_keepAliveMutex;

public:
	Cell* m_cells; // doesn't own this memory

	const WorldCoord m_x;
	const WorldCoord m_y;
	const ChunkCoord m_width;
	const ChunkCoord m_height;

	WorldCoord m_minX, m_minY,
		      m_maxX, m_maxY; // rect around cells to update

	std::atomic<uint32_t> m_filledCellCount;
	std::atomic<uint32_t> m_filledCellsWithProps;

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

	bool IsAllNoProps() const {
		return m_filledCellsWithProps == 0;
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
	      Cell& GetCellDirect(ChunkCoord x, ChunkCoord y)       { return m_cells[GetIndexDirect(x, y)]; }

	const Cell& GetCell(WorldCoord x, WorldCoord y) const { return m_cells[GetIndex(x, y)]; }
	      Cell& GetCell(WorldCoord x, WorldCoord y)       { return m_cells[GetIndex(x, y)]; }
		 Cell& GetCell(Index index)				    { return m_cells[index]; }

	void KeepAlive(
		WorldCoord x, WorldCoord y)
	{
		KeepAlive(GetIndex(x, y));
	}

	void KeepAlive(
		Index index);

	void SetCell(
		WorldCoord x, WorldCoord y,
		const Cell& cell,
		Tick currentTick)
	{
		SetCell(GetIndex(x, y), cell, currentTick);
	}

	void SetCell(
		Index index,
		const Cell& cell,
		Tick currentTick);

	void SetCellQueued(
		WorldCoord x, WorldCoord y,
		const Cell& cell);

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

	bool CommitMovedCells(
		Tick currentTick);

	void ResetRect();

	void SetCellData_dirty(
		Index index,
		const Cell& cell);
private:
	void UpdateRect(
		Index index);

	void SetCellData(
		Index index,
		const Cell& cell, // cant be const because of shared cleanup
		Tick currentTick);
};
