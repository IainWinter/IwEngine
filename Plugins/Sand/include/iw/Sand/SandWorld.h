#pragma once

#include "SandChunk.h"
#include "iw/util/algorithm/pair_hash.h"
#include <concurrent_unordered_map.h>

IW_PLUGIN_SAND_BEGIN

class SandWorld {
public:
	const size_t m_chunkWidth;
	const size_t m_chunkHeight;
	const double m_scale;

	std::vector<SandChunk*> m_chunks;
private:
	Concurrency::concurrent_unordered_map<std::pair<int, int>, SandChunk*, iw::pair_hash> m_chunkLookup;
	std::mutex m_chunkMutex;

public:
	IW_PLUGIN_SAND_API SandWorld(size_t chunkWidth, size_t chunkHeight, double scale);

	IW_PLUGIN_SAND_API Cell& GetCell(int x, int y);

	IW_PLUGIN_SAND_API void SetCell (int x, int y, const Cell& cell);
	IW_PLUGIN_SAND_API void MoveCell(int x, int y, int xto, int yto);

	IW_PLUGIN_SAND_API bool InBounds (int x, int y);
	IW_PLUGIN_SAND_API bool IsEmpty  (int x, int y);
	IW_PLUGIN_SAND_API void KeepAlive(int x, int y);

	IW_PLUGIN_SAND_API std::pair<int, int> GetChunkLocation(int x, int y);

	IW_PLUGIN_SAND_API void RemoveEmptyChunks();

	IW_PLUGIN_SAND_API SandChunk* GetChunk(int x, int y);
	IW_PLUGIN_SAND_API SandChunk* GetChunkDirect(std::pair<int, int> location);
private:
	SandChunk* CreateChunk(std::pair<int, int> location);
};

IW_PLUGIN_SAND_END
