#pragma once

#include "SandChunk.h"
#include <vector>
#include <unordered_map>

class SandWorker;

class SandWorld {
public:
	const WorldCoord m_chunkWidth;
	const WorldCoord m_chunkHeight;
	const double m_scale;
	bool m_fixedChunks;
	Tick m_currentTick;
	std::vector<SandChunk*> m_chunks;
	std::vector<SandWorker*> m_workers;
private:
	std::unordered_map<WorldCoords, Index, iw::pair_hash> m_chunkLookup;

	const size_t m_cellChunkSizeInBytes;
	iw::pool_allocator m_cells;

	std::mutex m_chunksMutex;

public:
	SandWorld(
		WorldCoord width,
		WorldCoord height,
		WorldCoord chunkWidth,
		WorldCoord chunkHeight,
		double scale)
		: m_chunkWidth (chunkWidth  / scale)
		, m_chunkHeight(chunkHeight / scale)
		, m_scale(scale)
		, m_cellChunkSizeInBytes(sizeof(Cell) * m_chunkWidth * m_chunkHeight)
		, m_cells(m_cellChunkSizeInBytes * 32)
		, m_fixedChunks(true)
		, m_currentTick(0)
	{
		for (WorldCoord x = 0; x <= width / m_chunkWidth;  x++)
		for (WorldCoord y = 0; y <= width / m_chunkHeight; y++) {
			CreateChunk({ x, y });
		}
	}

	SandWorld(
		WorldCoord chunkWidth,
		WorldCoord chunkHeight,
		double scale)
		: m_chunkWidth (chunkWidth  / scale)
		, m_chunkHeight(chunkHeight / scale)
		, m_scale(scale)
		, m_cellChunkSizeInBytes(sizeof(Cell) * m_chunkWidth* m_chunkHeight)
		, m_cells(m_cellChunkSizeInBytes * 32)
		, m_fixedChunks(false)
		, m_currentTick(0)
	{}

	void Reset() {
		m_cells.reset();
		m_chunks.clear();
		m_chunkLookup.clear();
	}

	void Step() {
		std::vector<WorldCoords> remove;

		for (SandChunk* chunk : m_chunks) {
			if (chunk->IsAllEmpty()) {
				remove.emplace_back(
					chunk->m_x % m_chunkWidth,
					chunk->m_y / m_chunkHeight);
			}
		}

		for (WorldCoords coords : remove) {
			m_chunks.erase(m_chunks.begin() + m_chunkLookup.at(coords));
			m_chunkLookup.erase(m_chunkLookup.find(coords));
		}
	}

	bool InBounds(
		WorldCoord x, WorldCoord y)
	{
		if (SandChunk* chunk = GetChunk(x, y)) {
			return chunk->InBounds(x, y);
		}

		return false;
	}

	bool IsEmpty(
		WorldCoord x, WorldCoord y)
	{
		return  InBounds(x, y)
			&& GetChunk(x, y)->IsEmpty(x, y);
	}

	const Cell& GetCell(
		WorldCoord x, WorldCoord y)
	{
		return GetChunk(x, y)->GetCell(x, y);
	}

	void SetCell(
		WorldCoord x, WorldCoord y,
		const Cell& cell)
	{
		GetChunk(x, y)->SetCell(x, y, cell, m_currentTick);
	}

	WorldCoords GetChunkLocation(
		WorldCoord x, WorldCoord y)
	{
		WorldCoord chunkX = (x < 0 ? x + 1 : x) / m_chunkWidth;
		WorldCoord chunkY = (y < 0 ? y + 1 : y) / m_chunkHeight;

		if (x < 0) chunkX--;
		if (y < 0) chunkY--;

		return { chunkX, chunkY };
	}

	std::vector<SandChunk*> GetVisibleChunks(
		WorldCoord x,  WorldCoord y,
		WorldCoord x2, WorldCoord y2)
	{
		std::vector<SandChunk*> visible;

		WorldCoords location  = GetChunkLocation(x, y);
		WorldCoords location2 = GetChunkLocation(x2, y2);

		WorldCoords itr = location;

		for (;                             itr.first  < location2.first;  itr. first++)
		for (itr.second = location.second; itr.second < location2.second; itr.second++) { // this is gross
			SandChunk* chunk = GetChunkFromLocation(itr, true);
			if (chunk) {
				visible.push_back(chunk);
			}
		}

		return visible;
	}

	SandChunk* GetChunk(
		WorldCoord x, WorldCoord y)
	{
		return GetChunkFromLocation(GetChunkLocation(x, y));
	}

	SandChunk* GetChunkDirect(
		WorldCoord x, WorldCoord y)
	{
		return GetChunkFromLocation({ x, y }, true);
	}
private:
	SandChunk* GetChunkFromLocation(
		WorldCoords location, bool dontCreateChunks = false)
	{
		if (    abs(location.first)  > 100
			|| abs(location.second) > 100)
		{
			return nullptr;
		}

		std::unique_lock lock(m_chunksMutex);

		auto itr = m_chunkLookup.find(location);
		if (itr == m_chunkLookup.end()) {
			if (m_fixedChunks || dontCreateChunks) {
				return nullptr;
			}

			return CreateChunk(location);
		}

		return m_chunks.at(itr->second);
	}

	SandChunk* CreateChunk(
		WorldCoords location)
	{
		SandChunk* chunk = new SandChunk(
			m_cells.alloc<Cell>(m_cellChunkSizeInBytes),
			m_chunkWidth * location.first, m_chunkHeight * location.second,
			m_chunkWidth, m_chunkHeight
		);

		m_chunkLookup.emplace(location, m_chunks.size()).first;
		m_chunks.emplace_back(chunk);

		return chunk;
	}
};
