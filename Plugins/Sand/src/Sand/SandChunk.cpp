#include "iw/Sand/SandChunk.h"

IW_PLUGIN_SAND_BEGIN

SandChunk::SandChunk(
	int width, int height,
	int x, int y)
	: m_width (width)
	, m_height(height)
	, m_x(x * width)
	, m_y(y * height)
	, m_filledCellCount(0)
{
	UpdateRect();
	UpdateRect();
}

// Setting & moving cells

void SandChunk::MoveCell(
	SandChunk* source,
	int x,   int y,
	int xto, int yto)
{
	std::unique_lock lock(m_changesMutex);

	m_changes.emplace_back(
		source, 
		source->GetIndex(x, y), 
		GetIndex(xto, yto)
	);
}

//void SandChunk::PushCell(
//	SandChunk* source, 
//	int x, int y, 
//	int xto, int yto)
//{
//	size_t src = source->GetIndex(x,   y);
//	size_t dst =         GetIndex(xto, yto);
//
//	std::unique_lock lock (        GetLock(dst));
//	std::unique_lock lock2(source->GetLock(src));
//
//	        SetCell(dst, source->GetCell(src, 0u), 1u); // there should be some system other than the fields that provide a past state
//	source->SetCell(src, Cell(), 0u);
//}

void SandChunk::CommitCells() {

#define _DEST 2

	// remove moves that have their destinations filled

	for (size_t i = 0; i < m_changes.size(); i++) {
		const Cell& dest = GetCell(std::get<_DEST>(m_changes[i]));

		if (dest.Type != CellType::EMPTY) {
			m_changes[i] = m_changes.back(); m_changes.pop_back();
			i--;
		}
	}

	// sort by destination

	std::sort(m_changes.begin(), m_changes.end(),
		[](auto& a, auto& b) { return std::get<_DEST>(a) < std::get<_DEST>(b); }
	);

	// pick random source for each destination

	size_t iprev = 0;

	m_changes.emplace_back(nullptr, -1, -1); // to catch final move

	for (size_t i = 0; i < m_changes.size() - 1; i++) {
		if (std::get<_DEST>(m_changes[i + 1]) != std::get<_DEST>(m_changes[i])) {
			size_t rand = iprev + iw::randi(i - iprev);

			auto [chunk, src, dst] = m_changes[rand];

				    SetCell(dst, chunk->GetCell(src));
			chunk->SetCell(src, Cell());

			iprev = i + 1;
		}
	}

	m_changes.clear();
#undef _DEST

	UpdateRect();

	//for (int x = m_minX; x < m_maxX; x++)
	//for (int y = m_minY; y < m_maxY; y++) {
	//	size_t i = x + y * m_width;

	//	Cell& cell = GetCell(i, 1u);

	//	if (cell.Type == CellType::EMPTY) continue;

	//	SetCell(i, cell, SandField::CELL);
	//	cell = Cell();
	//}
}

// Helpers

size_t SandChunk::GetIndex(int x, int y) {
	x -= m_x;
	y -= m_y;
	return x + y * m_width;
}

bool SandChunk::InBounds(int x, int y) {
	return x >= m_x && x < m_x + m_width
		&& y >= m_y && y < m_y + m_height;
}

bool SandChunk::IsEmpty(int x, int y) {
	//KeepAlive(x, y); // myabe keep all querys alive?

	std::unique_lock lock(GetLock(x, y, SandField::CELL));

	return GetCell(x, y).Type == CellType::EMPTY;
}

void SandChunk::KeepAlive(int x, int y) {
	KeepAlive(GetIndex(x, y));
}

void SandChunk::KeepAlive(size_t index) {
	int x = index % m_width;
	int y = index / m_width;

	std::unique_lock lock(m_workingRectMutex);

	m_minXw = iw::clamp(min(x - 2, m_minXw), 0, m_width);
	m_minYw = iw::clamp(min(y - 2, m_minYw), 0, m_height);
	m_maxXw = iw::clamp(max(x + 2, m_maxXw), 0, m_width);
	m_maxYw = iw::clamp(max(y + 2, m_maxYw), 0, m_height);
}

void SandChunk::UpdateRect() {
	// Update current; reset working
	m_minX = m_minXw;  m_minXw = m_width;
	m_minY = m_minYw;  m_minYw = m_height;
	m_maxX = m_maxXw;  m_maxXw = -1;
	m_maxY = m_maxYw;  m_maxYw = -1;	
}

IW_PLUGIN_SAND_END
