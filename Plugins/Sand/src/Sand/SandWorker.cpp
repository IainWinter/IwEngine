#include "iw/Sand/SandWorker.h"

IW_PLUGIN_SAND_BEGIN

SandWorker::SandWorker(
	SandWorld& world,
	SandChunk* chunk)
	: m_world(world)
	, m_chunk(chunk)
{}

// Updating cells

void SandWorker::UpdateChunk() {
	for (int x = m_chunk->m_minX; x < m_chunk->m_maxX; x++)
	for (int y = m_chunk->m_minY; y < m_chunk->m_maxY; y++) {
		Cell& cell = m_chunk->GetCell(x + y * m_chunk->m_width);

		if (cell.Props == CellProperties::NONE) continue;

		int px = x + m_chunk->m_x;
		int py = y + m_chunk->m_y;

		UpdateCell(px, py, cell);
	}
}

// Getting cells

Cell& SandWorker::GetCell(
	int x, int y)
{
	if (m_chunk->InBounds(x, y)) {
		return m_chunk->GetCell(x, y);
	}

	return m_world.GetCell(x, y);
}

// Setting & moving cells

void SandWorker::SetCell(
	int x, int y,
	const Cell& cell)
{
	if (m_chunk->InBounds(x, y)) {
		return m_chunk->SetCell(x, y, cell);
	}

	return m_world.SetCell(x, y, cell);
}

void SandWorker::MoveCell(
	int x,   int y,
	int xto, int yto)
{
	int pingX = 0, pingY = 0;

	if (x == m_chunk->m_x)                         pingX = -1;
	if (x == m_chunk->m_x + m_chunk->m_width  - 1) pingX =  1;
	if (y == m_chunk->m_y)                         pingY = -1;
	if (y == m_chunk->m_y + m_chunk->m_height - 1) pingY =  1;

	if (pingX != 0)               m_world.KeepAlive(x + pingX, y);
	if (pingY != 0)               m_world.KeepAlive(x,         y + pingY);
	if (pingX != 0 && pingY != 0) m_world.KeepAlive(x + pingX, y + pingY);

	if (    m_chunk->InBounds(x, y)
		&& m_chunk->InBounds(xto, yto))
	{
		return m_chunk->MoveCell(m_chunk, x, y, xto, yto);
	}

	return m_world.MoveCell(x, y, xto, yto);
}

// Helpers

bool SandWorker::InBounds(int x, int y) {
	return m_chunk->InBounds(x, y)
		|| m_world .InBounds(x, y);
}

bool SandWorker::IsEmpty(int x, int y) {
	if (m_chunk->InBounds(x, y)) {
		return m_chunk->IsEmpty(x, y);
	}

	return m_world.IsEmpty(x, y);
}

IW_PLUGIN_SAND_END
