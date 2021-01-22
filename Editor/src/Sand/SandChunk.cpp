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
	, m_lastTick(0)
{
	ResetRect();
}

SandChunk::SandChunk(
	Cell* cells, 
	WorldCoord x,     WorldCoord y,
	ChunkCoord width, ChunkCoord height,
	Tick currentTick)
	: m_cells(cells)
	, m_x(x)
	, m_y(y) 
	, m_width (width)
	, m_height(height)
	, m_filledCellCount(0)
	, m_lastTick(currentTick)
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
	, m_lastTick(copy.m_lastTick)
{
	ResetRect();
}

void SandChunk::KeepAlive(
	Index index)
{
	std::unique_lock lock(m_keepAliveMutex);
	m_keepAlive.push_back(index);
}

void SandChunk::SetCell(
	Index index,
	const Cell& cell,
	Tick currentTick)
{
	KeepAlive(index);

	std::unique_lock lock(m_setMutex);
	SetCellData(index, cell, currentTick);
	m_lastTick = currentTick;
}

void SandChunk::SetCellQueued(
	WorldCoord x, WorldCoord y,
	const Cell& cell)
{
	Index index = GetIndex(x, y);
	KeepAlive(index);

	std::unique_lock lock(m_setChangesMutex);
	m_setQueue.emplace_back(index, cell);
}

void SandChunk::MoveCell(
	SandChunk* sourceChunk,
	WorldCoord x,   WorldCoord y,
	WorldCoord xTo, WorldCoord yTo)
{
	std::unique_lock lock(m_changesMutex);
	m_moveQueue.emplace_back(
		sourceChunk,
		sourceChunk->GetIndex(x, y),
		GetIndex(xTo, yTo)
	);
}

bool SandChunk::CommitMovedCells(
	Tick currentTick)
{
	bool changed = m_keepAlive.size() > 0 || m_setQueue.size() > 0 || m_moveQueue.size() > 0;
	if (!changed) return false;

	ResetRect();

	// KEEP ALIVES

	for (Index index : m_keepAlive) {
		UpdateRect(index);
	}

	m_keepAlive.clear();

	// SETS

	for (size_t i = 0; i < m_setQueue.size(); i++) {
		auto& [index, cell] = m_setQueue[i];
		if (GetCell(index).Precedence >= cell.Precedence) {
			m_setQueue[i] = m_setQueue.back(); m_setQueue.pop_back();
			i--;
		}
	}

	std::sort(m_setQueue.begin(), m_setQueue.end(),
		[=](auto& a, auto& b) { return std::get<1>(a).Precedence < std::get<1>(b).Precedence; }
	);

	for (auto& [index, cell] : m_setQueue) {
		SetCellData(index, cell, currentTick);
	}

	m_setQueue.clear();

	// MOVES

	// remove moves that have their destinations filled

	constexpr size_t _CHUNK = 0;
	constexpr size_t _SRC   = 1;
	constexpr size_t _DEST  = 2;

	for (size_t i = 0; i < m_moveQueue.size(); i++) {
		Cell& src  = m_cells[std::get<_SRC> (m_moveQueue[i])];
		Cell& dest = m_cells[std::get<_DEST>(m_moveQueue[i])];
		
		if (   dest.Type != CellType::EMPTY
			&& dest.Precedence >= src.Precedence)
		{
			m_moveQueue[i] = m_moveQueue.back(); m_moveQueue.pop_back();
			i--;
		}
	}

	if (m_moveQueue.size() == 0) return changed;

	// sort by destination

	std::sort(m_moveQueue.begin(), m_moveQueue.end(),
		[=](auto& a, auto& b) { return std::get<2>(a) < std::get<2>(b); } // cant pass _DEST (constexpr) I guess
	);

	// pick random source for each destination

	size_t ip = 0;

	m_moveQueue.emplace_back(nullptr, -1, -1); // to catch last change

	for (size_t i = 0; i < m_moveQueue.size() - 1; i++) {
		if (std::get<_DEST>(m_moveQueue[i + 1]) != std::get<_DEST>(m_moveQueue[i])) {
			auto [sourceChunk, src, dest] = m_moveQueue[ip + iw::randi(i - ip)];

			UpdateRect(dest);

			if (!sourceChunk) {
				LOG_ERROR << "Null source chunk in move!"; // not cause of crash, must be chunks that has been deleted but idk how that would be because they are only deleted after calling this function
			}

			SetCellData(dest, sourceChunk->GetCell(src), currentTick);
			sourceChunk->SetCell(src, Cell::GetDefault(CellType::EMPTY), currentTick);

			ip = i + 1;
		}
	}

	m_moveQueue.clear();
	m_lastTick = currentTick;

	return changed;
}

// Private

void SandChunk::ResetRect() {
	m_minX = m_width;
	m_minY = m_height;
	m_maxX = -1;
	m_maxY = -1;
}

void SandChunk::UpdateRect(
	Index index) // damn this doesnt really work :< idk what other approachs there are
{
	int x = index % m_width;
	int y = index / m_height;

	//if (x <= m_minX) m_minX = iw::clamp<WorldCoord>(x - 2, 0, m_width);
	//if (x >= m_maxX) m_maxX = iw::clamp<WorldCoord>(x + 2, 0, m_width);

	//if (y <= m_minY) m_minY = iw::clamp<WorldCoord>(y - 2, 0, m_height);
	//if (y >= m_maxY) m_maxY = iw::clamp<WorldCoord>(y + 2, 0, m_height);
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
		
	else if (cell.Type != CellType::EMPTY
	      && dest.Type == CellType::EMPTY)
	{
		++m_filledCellCount;
	}

	if (   cell.Props == CellProperties::NONE
		&& dest.Props != CellProperties::NONE)
	{
		--m_filledCellsWithProps;
	}

	else if (cell.Props != CellProperties::NONE
		  && dest.Props == CellProperties::NONE)
	{
		++m_filledCellsWithProps;
	}

	float posX = cell.pX;
	float posY = cell.pY;

	if (!cell.UseFloatingPosition) {
		posX = int(index) % m_width  + m_x;
		posY = int(index) / m_height + m_y;
	}

	if (dest.Share) {
		std::unique_lock lock(dest.Share->m_userMutex);

		auto& [count, location] = dest.Share->UserTypeCounts[dest.Type];
		location = location * count - iw::vector2(dest.pX, dest.pY);
		count--;
		location /= (count == 0 ? 1 : count);

		dest.Share->m_users.erase(&dest);
		dest.Share->UserCount--;
	}

	if (cell.Share) {
		std::unique_lock lock(cell.Share->m_userMutex);

		auto& [count, location] = cell.Share->UserTypeCounts[cell.Type];
		location = location * count + iw::vector2(posX, posY);
		count++;
		location /= count;

		cell.Share->m_users.emplace(&dest);
		cell.Share->UserCount++;
	}

	dest = cell;
	dest.LastUpdateTick = currentTick;

	dest.pX = posX;
	dest.pY = posY;
}
