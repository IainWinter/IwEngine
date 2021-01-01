#pragma once

#include "SandChunk.h"
#include <vector>
#include <unordered_map>

struct SandWorker;

class SandWorld {
public:
	const ChunkCoord m_chunkWidth;
	const ChunkCoord m_chunkHeight;
	const double m_scale;
	bool m_fixedChunks;
	Tick m_currentTick;
	std::vector<SandChunk> m_chunks;
	std::vector<SandWorker> m_workers;
private:
	std::unordered_map<WorldCoords, Index, iw::pair_hash> m_chunkLookup;

	iw::pool_allocator m_cells;
	const size_t m_cellChunkSizeInBytes;

public:
	SandWorld(
		WorldCoord width,
		WorldCoord height,
		ChunkCoord chunkWidth,
		ChunkCoord chunkHeight,
		double scale)
		: m_chunkWidth (chunkWidth  / scale)
		, m_chunkHeight(chunkHeight / scale)
		, m_scale(scale)
		, m_cellChunkSizeInBytes(sizeof(Cell) * m_chunkWidth * m_chunkHeight)
		, m_cells(m_cellChunkSizeInBytes)
		, m_fixedChunks(true)
	{
		for (WorldCoord x = 0; x < width / m_chunkWidth;  x++)
		for (WorldCoord y = 0; y < width / m_chunkHeight; y++) {
			//CreateChunk(x, y);
		}
	}

	SandWorld(
		ChunkCoord chunkWidth,
		ChunkCoord chunkHeight,
		double scale)
		: m_chunkWidth (chunkWidth  / scale)
		, m_chunkHeight(chunkHeight / scale)
		, m_scale(scale)
		, m_cellChunkSizeInBytes(sizeof(Cell)* m_chunkWidth* m_chunkHeight)
		, m_cells(m_cellChunkSizeInBytes)
		, m_fixedChunks(false)
	{}

	bool InBounds(
		WorldCoord x, WorldCoord y)
	{
		if (SandChunk* chunk = GetChunk(x, y)) {
			return chunk->InBounds(GetChunkCoords(x, y));
		}

		return false;
	}

	bool IsEmpty(
		WorldCoord x, WorldCoord y)
	{
		return  InBounds(x, y)
			&& GetChunk(x, y)->IsEmpty(GetChunkCoords(x, y));
	}

	const Cell& GetCell(
		WorldCoord x, WorldCoord y)
	{
		return GetChunk(x, y)->GetCell(GetChunkCoords(x, y));
	}

	void SetCell(
		WorldCoord x, WorldCoord y,
		const Cell& cell)
	{
		GetChunk(x, y)->SetCell(GetChunkCoords(x, y), cell, m_currentTick);
	}

	ChunkCoords GetChunkCoords(
		WorldCoord x, WorldCoord y)
	{
		ChunkCoord chunkX = x % m_chunkWidth;
		ChunkCoord chunkY = y % m_chunkHeight;

		if (x < 0) chunkX += m_chunkWidth;
		if (y < 0) chunkY += m_chunkHeight;

		return { chunkX, chunkY };
	}

	std::vector<SandChunk*> GetVisibleChunks(
		int x, int y,
		int x2, int y2)
	{
		std::vector<SandChunk*> visible;

		auto [chunkX,  chunkY]  = GetChunkCoords(x, y);
		auto [chunkX2, chunkY2] = GetChunkCoords(x2, y2);

		for (int cx = chunkX; cx < chunkX2; cx++)
		for (int cy = chunkY; cy < chunkY2; cy++) {
			SandChunk* chunk = GetChunk(cx, cy);
			if (chunk) {
				visible.push_back(chunk);
			}
		}

		return visible;
	}
private:
	SandChunk* GetChunk(
		WorldCoord x, WorldCoord y)
	{
		if (abs(x) > 100 || abs(y) > 100) {
			return nullptr;
		}

		WorldCoords coords = { x, y };

		auto itr = m_chunkLookup.find(coords);
		if (itr == m_chunkLookup.end()) {
			if (m_fixedChunks) {
				return nullptr;
			}

			itr = m_chunkLookup.emplace(coords, m_chunks.size()).first;

			m_chunks.emplace_back(
				m_cells.alloc<Cell>(m_cellChunkSizeInBytes),
				x, y, m_chunkWidth, m_chunkWidth
			);
		}

		return &m_chunks.at(itr->second);
	}
};
