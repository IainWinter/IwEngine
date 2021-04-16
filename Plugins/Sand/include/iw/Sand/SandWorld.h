#pragma once

#include "SandChunk.h"
#include "iw/util/algorithm/pair_hash.h"
#include "iw/util/memory/pool_allocator.h"
#include <concurrent_unordered_map.h>

IW_PLUGIN_SAND_BEGIN

class SandWorld {
public:
	const size_t m_chunkWidth;
	const size_t m_chunkHeight;
	const double m_scale;

	bool m_expandWorld;

	std::vector<SandChunk*> m_chunks;
private:
	Concurrency::concurrent_unordered_map<std::pair<int, int>, SandChunk*, iw::pair_hash> m_chunkLookup;
	std::mutex m_chunkMutex;

	struct Field {
		iw::pool_allocator* memory;
		size_t size;
	};

	std::vector<Field> m_fields;

public:

	template<typename _t>
	void AddField() {
		iw::pool_allocator* field = new iw::pool_allocator(16 * sizeof(_t) * m_chunkWidth * m_chunkHeight);

		m_fields.push_back({ field, sizeof(_t) });

		//for (SandChunk* chunk : m_chunks) {
		//	
		//}
	}

	IW_PLUGIN_SAND_API SandWorld(size_t chunkWidth, size_t chunkHeight, double scale);
	IW_PLUGIN_SAND_API SandWorld(size_t screenSizeX, size_t screenSizeY, size_t numberOfChunksX, size_t numberOfChunksY, double scale);

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
