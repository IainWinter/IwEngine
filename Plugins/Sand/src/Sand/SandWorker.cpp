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

	Cell* cells = m_chunk->GetField().cells;

	bool forwardX = true;
	bool forwardY = true;

	//switch (m_world.m_frameCount % 2) {
	//	case 0: {
	//		break;
	//	}
	//	case 1: {
	//		forwardX = false;
	//		forwardY = false;
	//		break;
	//	}
	//	//case 2: {
	//	//	forwardY = false;
	//	//	break;
	//	//}
	//	//case 3: {
	//	//	forwardX = false;
	//	//	forwardY = false;
	//	//	break;
	//	//}
	//}

	for (int x = m_chunk->m_minX; x < m_chunk->m_maxX; x++)
	for (int y = m_chunk->m_minY; y < m_chunk->m_maxY; y++)
	{
		int xx = forwardX ? x : m_chunk->m_maxX - x - 1;
		int yy = forwardY ? y : m_chunk->m_maxY - y - 1;

		Cell& cell = cells[xx + yy * m_chunk->m_width];

		if (cell.Props == CellProperties::NONE) continue;

		int px = xx + m_chunk->m_x;
		int py = yy + m_chunk->m_y;

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
		m_chunk->SetCell(x, y, cell);
		return;
	}

	m_world.SetCell(x, y, cell);
}

void _KeepAlive(SandWorld& world, SandChunk* chunk, int x, int y) {
		int pingX = 0, pingY = 0;

	if (x == chunk->m_x)                       pingX = -1;
	if (x == chunk->m_x + chunk->m_width  - 1) pingX =  1;
	if (y == chunk->m_y)                       pingY = -1;
	if (y == chunk->m_y + chunk->m_height - 1) pingY =  1;

	if (pingX != 0)               world.KeepAlive(x + pingX, y);
	if (pingY != 0)               world.KeepAlive(x,         y + pingY);
	if (pingX != 0 && pingY != 0) world.KeepAlive(x + pingX, y + pingY);
}

void SandWorker::MoveCell(
	int x,   int y,
	int xto, int yto)
{
	_KeepAlive(m_world, m_chunk, x, y);

	if (    m_chunk->InBounds(x, y)
		&& m_chunk->InBounds(xto, yto))
	{
		return m_chunk->MoveCell(m_chunk, x, y, xto, yto);
	}

	return m_world.MoveCell(x, y, xto, yto);
}

void SandWorker::PushCell(
	int x,   int y,
	int xto, int yto)
{
	_KeepAlive(m_world, m_chunk, x, y);

	if (   m_chunk->InBounds(x, y)
		&& m_chunk->InBounds(xto, yto))
	{
		return m_chunk->PushCell(m_chunk, x, y, xto, yto);
	}

	return m_world.PushCell(x, y, xto, yto);
}

void SandWorker::KeepAlive(int x, int y) {
	if (m_chunk->InBounds(x, y)) {
		return m_chunk->KeepAlive(x, y);
	}

	return m_world.KeepAlive(x, y);
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
