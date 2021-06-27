#include "iw/Sand/SandWorld.h"

IW_PLUGIN_SAND_BEGIN

SandWorld::SandWorld(
	size_t chunkWidth,
	size_t chunkHeight,
	double scale)
	: m_chunkWidth (chunkWidth  / scale)
	, m_chunkHeight(chunkHeight / scale)
	, m_scale(scale)
	, m_expandWorld(true)
{
	m_batches.resize(m_batchGridSize * m_batchGridSize);
}

SandWorld::SandWorld(
	size_t screenSizeX,
	size_t screenSizeY,
	size_t numberOfChunksX,
	size_t numberOfChunksY,
	double scale)
	: m_chunkWidth (screenSizeX / numberOfChunksX / scale)
	, m_chunkHeight(screenSizeY / numberOfChunksY / scale)
	, m_scale(scale)
	, m_expandWorld(true)
{
	m_batches.resize(m_batchGridSize * m_batchGridSize);

	for(size_t x = 0; x < numberOfChunksX; x++)
	for(size_t y = 0; y < numberOfChunksY; y++) {
		CreateChunk({ x, y });
	}

	m_expandWorld = false;
}

// Getting cells

Cell& SandWorld::GetCell(
	int x, int y,
	SandField field)
{
	return GetChunk(x, y)->GetCell(x, y, field);
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

//void SandWorld::PushCell(
//	int x,   int y,
//	int xto, int yto)
//{
//	if (SandChunk* src  = GetChunk(x, y))
//	if (SandChunk* dest = GetChunk(xto, yto)) {
//		dest->PushCell(src, x, y, xto, yto);
//	}
//}

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
	for (auto& chunks : m_batches)
	for (size_t i = 0; i < chunks.size(); i++) {
		SandChunk* chunk = chunks.at(i);

		if (    chunk->m_filledCellCount == 0  // delay by 1 tick
			&& !chunk->m_deleteMe)
		{
			chunk->m_deleteMe = true;
			continue;
		}

		if (chunk->m_deleteMe) {
			m_chunkLookup.unsafe_erase(GetChunkLocation(chunk->m_x, chunk->m_y));
			chunks[i] = chunks.back(); chunks.pop_back();
			i--;

			for (size_t i = 0; i < m_fields.size(); i++) {
				FieldConfig& field = m_fields[i];
				
				field.memory->free(
					chunk->m_fields[i].cells,
					FieldSize(field.cellSize)
				);

				// temp, should be field.memory->free as well
				if (field.hasLocks) {
					delete[] chunk->m_fields[i].locks;
				}
			}
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
	if (!m_expandWorld) return nullptr;

	auto [lx, ly] = location;

	if (   lx < -50 || ly < -50
		|| lx >  50 || ly >  50) // could pass in a world limit to constructor
	{
		return nullptr;
	}

	SandChunk* chunk = new SandChunk(m_chunkWidth, m_chunkHeight, lx, ly);

	for (FieldConfig& field : m_fields) {
		AddFieldToChunk(chunk, field);
	}

	m_chunkLookup.insert({ location, chunk });

	int c = m_batchGridSize;

	int batch = (c + lx % c) % c
		      + (c + ly % c) % c * c;

	{
		std::unique_lock lock(m_chunkMutex);
		m_batches.at(batch).push_back(chunk);
	}

	return chunk;
}

IW_PLUGIN_SAND_END
