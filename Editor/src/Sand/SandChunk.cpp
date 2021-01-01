#include "Sand/SandChunk.h"
#include <algorithm>

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

SandChunk& SandChunk::operator=(
	const SandChunk& copy)
{
	m_cells           = copy.m_cells;
	m_filledCellCount = copy.m_filledCellCount.load();

	ResetRect();

	return *this;
}

void SandChunk::SetCell(
	ChunkCoords coords,
	const Cell& cell,
	Tick currentTick)
{
	Index index = GetIndex(coords);
	
	SetCellData(index, cell, currentTick);
	m_changes.emplace_back(index, index); // for rect update
}

void SandChunk::MoveCell(
	ChunkCoords from,
	ChunkCoords to)
{
	m_changes.emplace_back(
		GetIndex(to),
		GetIndex(from)
	);
}

void SandChunk::CommitMovedCells(
	Tick currentTick)
{
	ResetRect();

	// remove moves that have their destinations filled
	//	also do special update for 'set' cells (dest == source)
	for (size_t i = 0; i < m_changes.size(); i++) {
		if (m_cells[m_changes[i].first].Type != CellType::EMPTY) {
			if (m_changes[i].first == m_changes[i].second) {
				UpdateRect(m_changes[i].first % m_width,
					       m_changes[i].first / m_width);
			}

			m_changes[i] = m_changes.back(); m_changes.pop_back();
			i--;
		}
	}

	// sort by destination

	std::sort(m_changes.begin(), m_changes.end(),
		[](auto& a, auto& b) { return a.first < b.first; }
	);

	// pick random source for each destination

	size_t ip = 0;

	m_changes.emplace_back(-1, -1); // to catch last change

	for (size_t i = 0; i < m_changes.size(); i++) {
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
	ChunkCoord x, ChunkCoord y)
{
	if (x <= m_minX) {
		m_minX = iw::clamp<uint16_t>(x - 2, 0, m_width);
	}

	if (y <= m_minY) {
		m_minY = iw::clamp<uint16_t>(y - 2, 0, m_height);
	}

	if (x >= m_maxX) {
		m_maxX = iw::clamp<uint16_t>(x + 2, 0, m_width);
	}

	if (y >= m_maxY) {
		m_maxY = iw::clamp<uint16_t>(y + 2, 0, m_height);
	}
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
