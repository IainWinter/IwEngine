#include "iw/Sand/SandWorld.h"

IW_PLUGIN_SAND_BEGIN

SandWorld::SandWorld(
	size_t chunkWidth,
	size_t chunkHeight,
	double scale)
	: m_chunkWidth (chunkWidth  / scale)
	, m_chunkHeight(chunkHeight / scale)
	, m_scale(scale)
{}

// Getting cells

Cell& SandWorld::GetCell(
	int x, int y)
{
	return GetChunk(x, y)->GetCell(x, y);
}

// Setting & moving cells

void SandWorld::SetCell(
	int x, int y,
	const Cell& cell)
{
	if (SandChunk* chunk = GetChunk(x, y)) {
		chunk->SetCell(x, y, cell);
	}
}

void SandWorld::MoveCell(
	int x,   int y,
	int xto, int yto)
{
	if (SandChunk* src  = GetChunk(x, y))
	if (SandChunk* dest = GetChunk(xto, yto)) {
		dest->MoveCell(src, x, y, xto, yto);
	}
}

// Helpers

bool SandWorld::InBounds(
	int x, int y)
{
	if (SandChunk* chunk = GetChunk(x, y)) {
		return chunk->InBounds(x, y);
	}

	return false;
}

bool SandWorld::IsEmpty(
	int x, int y)
{
	return InBounds(x, y)
		&& GetChunk(x, y)->IsEmpty(x, y);
}

void SandWorld::KeepAlive(
	int x, int y)
{
	if (SandChunk* chunk = GetChunk(x, y)) {
		chunk->KeepAlive(x, y);
	}
}

std::pair<int, int> SandWorld::GetChunkLocation(
	int x, int y)
{
	return {
		floor(float(x) / m_chunkWidth),
		floor(float(y) / m_chunkHeight),
	};
}

void SandWorld::RemoveEmptyChunks() {
	for (size_t i = 0; i < m_chunks.size(); i++) {
		SandChunk* chunk = m_chunks.at(i);

		if (chunk->m_filledCellCount == 0) {
			m_chunkLookup.unsafe_erase(GetChunkLocation(chunk->m_x, chunk->m_y));
			m_chunks[i] = m_chunks.back(); m_chunks.pop_back();
			i--;

			delete chunk;
		}
	}
}

SandChunk* SandWorld::GetChunk(
	int x, int y)
{
	auto location = GetChunkLocation(x, y);

	SandChunk* chunk = GetChunkDirect(location);
	if (!chunk) {
		chunk = CreateChunk(location);
	}

	return chunk;
}

SandChunk* SandWorld::GetChunkDirect(
	std::pair<int, int> location)
{
	auto itr = m_chunkLookup.find(location);
	if (itr == m_chunkLookup.end()) {
		return nullptr;
	}

	return itr->second;
}

SandChunk* SandWorld::CreateChunk(
	std::pair<int, int> location)
{
	auto [lx, ly] = location;

	if (    lx < -50 || ly < -50
		    || lx >  50 || ly >  50) // could pass in a world limit to constructor
	{
		return nullptr;
	}

	SandChunk* chunk = new SandChunk(
		m_chunkWidth, m_chunkHeight, lx, ly);

	m_chunkLookup.insert({ location, chunk });

	{
		std::unique_lock lock(m_chunkMutex);
		m_chunks.push_back(chunk);
	}

	return chunk;
}

IW_PLUGIN_SAND_END
