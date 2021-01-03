#pragma once

#include "Cell.h"
#include <vector>
#include <atomic>
#include <mutex>

struct SandChunk {
private:
	std::vector<IndexPair> m_changes;    // destination, source
	std::vector<Index>     m_setChanges;

	std::mutex m_setChangesMutex;
public:
	Cell* m_cells; // doesn't own this memory

	const WorldCoord m_x;
	const WorldCoord m_y;
	const ChunkCoord m_width;
	const ChunkCoord m_height;

	WorldCoord m_minX, m_minY,
		      m_maxX, m_maxY; // rect around cells to update

	std::atomic<uint32_t> m_filledCellCount;

	SandChunk();

	SandChunk(
		Cell* cells,
		WorldCoord x,     WorldCoord y,
		ChunkCoord width, ChunkCoord height);

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

		if (x < 0 || y < 0 || x >= m_width || y >= m_height) {
			return 0;
		}

		return x + y * m_width; // map coords here
	}

	Index GetIndexDirect(
		ChunkCoord x, ChunkCoord y) const
	{
		return x + y * m_width; // map coords here
	}

	const Cell& GetCellDirect(ChunkCoord x, ChunkCoord y) const { return m_cells[GetIndexDirect(x, y)]; }


	const Cell& GetCell(WorldCoord x, WorldCoord y) const { return m_cells[GetIndex(x, y)]; }
	      Cell& GetCell(WorldCoord x, WorldCoord y)       { return m_cells[GetIndex(x, y)]; }

	void SetCell(
		WorldCoord x, WorldCoord y,
		const Cell& cell,
		Tick currentTick);

	void MoveCell(
		WorldCoord x, WorldCoord   y,
		WorldCoord xTo, WorldCoord yTo);

	void CommitMovedCells(
		Tick currentTick);
private:
	void ResetRect();

	void UpdateRect(
		WorldCoord x, WorldCoord y);

	void MoveCellData(
		IndexPair change,
		Tick currentTick);

	void SetCellData(
		Index index,
		const Cell& cell,
		Tick currentTick);
};
