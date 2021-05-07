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

// Getting cells

//Cell& SandChunk::GetCell(int x, int y) { return GetCell(GetIndex(x, y)); }
//Cell& SandChunk::GetCell(size_t index) { return m_cells[index]; }

// Setting & moving cells

void SandChunk::SetCell(
	int x, int y,
	const Cell& cell)
{
	SetCell(GetIndex(x, y), cell);
}

void SandChunk::SetCell(
	size_t index,
	const Cell& cell)
{
	Cell& dest = GetCell(index);// m_cells[index];

	if (   dest.Type == CellType::EMPTY
		&& cell.Type != CellType::EMPTY) // Filling a cell
	{
		std::unique_lock lock(m_filledCellCountMutex);
		m_filledCellCount++;
	}

	else
	if (   dest.Type != CellType::EMPTY
		&& cell.Type == CellType::EMPTY) // Removing a filled cell
	{
		std::unique_lock lock(m_filledCellCountMutex);
		m_filledCellCount--;
	}

	dest = cell;

	// set location if its not set, this is a hack

	if (dest.x == 0 && dest.y == 0) {
		dest.x = m_x + index % m_width;
		dest.y = m_y + index / m_width;
	}

	// set location everytime it changed whole number

	dest.x = float(dest.x - int(dest.x)) + index % m_width + m_x;
	dest.y = float(dest.y - int(dest.y)) + index / m_width + m_y;

	KeepAlive(index);
}

void SandChunk::MoveCell(
	SandChunk* source,
	int x,   int y,
	int xto, int yto)
{
	size_t src = source->GetIndex(x,   y);
	size_t dst =         GetIndex(xto, yto);

	std::unique_lock lock(m_changesMutex);

	m_changes.emplace_back(source, src, dst);
}

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
