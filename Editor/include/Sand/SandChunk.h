#pragma once

#include "Cell.h"
#include <vector>
#include <atomic>

struct SandChunk {
private:
	std::vector<IndexPair> m_changes; // destination, source
	std::atomic<uint32_t> m_filledCellCount;
public:
	Cell* m_cells; // doesn't own this memory

	const WorldCoord m_x;
	const WorldCoord m_y;
	const ChunkCoord m_width;
	const ChunkCoord m_height;

	ChunkCoord m_minX, m_minY,
		      m_maxX, m_maxY; // rect around cells to update

	SandChunk();
	SandChunk(
		Cell* cells,
		WorldCoord x,     WorldCoord y,
		ChunkCoord width, ChunkCoord height);

	SandChunk& operator=(const SandChunk& copy);

	bool IsAllEmpty() const {
		return m_filledCellCount == 0;
	}

	bool	InBounds(
		ChunkCoords coords) const
	{
		return  coords.first < m_width
			&& coords.second < m_height;
	}

	bool	InBounds(
		WorldCoord x, WorldCoord y) const
	{
		return  x > m_x && x < m_x + m_width
			&& y > m_y && y < m_y + m_height;
	}

	bool IsEmpty(
		ChunkCoords coords) const
	{
		return GetCell(coords).Type == CellType::EMPTY;
	}

	Index GetIndex(
		ChunkCoords coords) const
	{
		return coords.first + coords.second * m_width;
	}

	const Cell& GetCell(ChunkCoords coords) const { return m_cells[GetIndex(coords)]; }
	      Cell& GetCell(ChunkCoords coords)       { return m_cells[GetIndex(coords)]; }

	void SetCell(
		ChunkCoords coords,
		const Cell& cell,
		Tick currentTick);

	void MoveCell(
		ChunkCoords from,
		ChunkCoords to);

	void CommitMovedCells(
		Tick currentTick);
private:
	void ResetRect();

	void UpdateRect(
		ChunkCoord x, ChunkCoord y);

	void MoveCellData(
		IndexPair change,
		Tick currentTick);

	void SetCellData(
		Index index,
		const Cell& cell,
		Tick currentTick);
};
