#include "Sand/SandChunk.h"
#include <algorithm>
#include <iw/log/logger.h>

SandChunk::SandChunk()
	: m_cells(nullptr)
	, m_x(0)
	, m_y(0)
	, m_width (0)
	, m_height(0)
	, m_filledCellCount(0)
{
	ResetRect();
}

SandChunk::SandChunk(
	Cell* cells, 
	WorldCoord x,     WorldCoord y,
	ChunkCoord width, ChunkCoord height)
	: m_cells(cells)
	, m_x(x)
	, m_y(y) 
	, m_width (width)
	, m_height(height)
	, m_filledCellCount(0)
{
	ResetRect();
}

SandChunk::SandChunk(
	const SandChunk& copy)
	: m_cells(copy.m_cells)
	, m_x(copy.m_x)
	, m_y(copy.m_y)
	, m_width (copy.m_width)
	, m_height(copy.m_height)
	, m_filledCellCount(copy.m_filledCellCount.load())
{
	ResetRect();
}

void SandChunk::SetCell(
	WorldCoord x, WorldCoord y,
	const Cell& cell,
	Tick currentTick)
{
	Index index = GetIndex(x, y);
	
	std::unique_lock lock(m_setChangesMutex);

	SetCellData(index, cell, currentTick);
	m_setChanges.push_back(index); // for rect update
}

void SandChunk::MoveCell(
	WorldCoord x, WorldCoord   y,
	WorldCoord xTo, WorldCoord yTo)
{
	m_changes.emplace_back(
		GetIndex(xTo, yTo),
		GetIndex(x,   y)
	);
}

void SandChunk::CommitMovedCells(
	Tick currentTick)
{
	ResetRect();

	for (Index index : m_setChanges) {
		UpdateRect(index % m_width, index / m_width);
	}

	m_setChanges.clear();

	// remove moves that have their destinations filled
	//	also do special update for 'set' cells (dest == source)
	for (size_t i = 0; i < m_changes.size(); i++) {
		if (m_cells[m_changes[i].first].Type != CellType::EMPTY) {
			m_changes[i] = m_changes.back(); m_changes.pop_back();
			i--;
		}
	}

	if (m_changes.size() == 0) return;

	// sort by destination

	std::sort(m_changes.begin(), m_changes.end(),
		[](auto& a, auto& b) { return a.first < b.first; }
	);

	// pick random source for each destination

	size_t ip = 0;

	m_changes.emplace_back(-1, -1); // to catch last change

	for (size_t i = 0; i < m_changes.size() - 1; i++) {
		if (m_changes[i + 1].first != m_changes[i].first) {
			MoveCellData(m_changes[ip + iw::randi(i - ip)], currentTick);
			ip = i + 1;
		}
	}

	m_changes.clear();
}

// Private

void SandChunk::ResetRect() {
	m_minX = m_width;
	m_minY = m_height;
	m_maxX = -1;
	m_maxY = -1;
}

void SandChunk::UpdateRect(
	WorldCoord x, WorldCoord y)
{
	if (x <= m_minX) m_minX = iw::clamp<WorldCoord>(x - 2, 0, m_width);
	if (x >= m_maxX) m_maxX = iw::clamp<WorldCoord>(x + 2, 0, m_width);

	if (y <= m_minY) m_minY = iw::clamp<WorldCoord>(y - 2, 0, m_height);
	if (y >= m_maxY) m_maxY = iw::clamp<WorldCoord>(y + 2, 0, m_height);
}

void SandChunk::MoveCellData(
	IndexPair change,
	Tick currentTick)
{
	uint32_t dest = change.first;
	uint32_t src  = change.second;

	Cell& from = m_cells[src];

	SetCellData(dest, from, currentTick);

	from = Cell::GetDefault(CellType::EMPTY);

	UpdateRect(src  % m_width, src  / m_width);
	UpdateRect(dest % m_width, dest / m_width);
}

void SandChunk::SetCellData(
	Index index,
	const Cell& cell,
	Tick currentTick)
{
	Cell& dest = m_cells[index];

	if (   cell.Type == CellType::EMPTY
		&& dest.Type != CellType::EMPTY)
	{
		--m_filledCellCount;
	}
		
	else if (  cell.Type != CellType::EMPTY
			&& dest.Type == CellType::EMPTY)
	{
		++m_filledCellCount;
	}

	dest = cell;
	dest.LastUpdateTick = currentTick;
}
