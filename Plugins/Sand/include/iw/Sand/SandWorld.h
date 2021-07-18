#pragma once

#include "SandChunk.h"
#include "iw/util/algorithm/pair_hash.h"
#include "iw/util/memory/pool_allocator.h"
#include <concurrent_unordered_map.h>

IW_PLUGIN_SAND_BEGIN

struct SandWorkerBuilderBase;

class SandWorld {
public:
	const size_t m_chunkWidth;
	const size_t m_chunkHeight;
	const double m_scale;

	const int m_batchGridSize = 2;

	bool m_expandWorld;
	size_t m_frameCount = 0;

	std::function<void(SandChunk*)> CreatedChunkCallback;
	std::function<void(SandChunk*)> RemovedChunkCallback;

	std::vector<std::vector<SandChunk*>> m_batches;
	std::vector<SandWorkerBuilderBase*>  m_workers;
private:
	Concurrency::concurrent_unordered_map<std::pair<int, int>, SandChunk*, iw::pair_hash> m_chunkLookup;
	std::mutex m_chunkMutex;

	struct FieldConfig {
		iw::pool_allocator* memory;
		size_t cellSize;
		bool hasLocks;

		onSetCell_func onSetCell;
	};

	std::vector<FieldConfig> m_fields;

public:

	template<typename _t>
	void AddField(
		bool hasLocks = true,
		onSetCell_func onSetCell = onSetCell_func())
	{
		//size_t cellSize = FieldSize<_t>();
		//size_t lockSize = hasLocks ? FieldSize<std::mutex>() : 0u;

		m_fields.push_back({
			new iw::pool_allocator(16 * FieldSize<_t>()/*(cellSize + lockSize)*/),
			sizeof(_t),
			hasLocks,
			onSetCell
		});

		for (auto& chunks : m_batches)
		for (SandChunk* chunk : chunks) {
			AddFieldToChunk(chunk, m_fields.back());
			//chunk->AddField(m_fields.back().memory->alloc(fieldSize));
		}
	}

	template<typename _t>
	size_t FieldSize() {
		return FieldSize(sizeof(_t));
	}

	inline size_t FieldSize(size_t size) {
		return size * m_chunkWidth * m_chunkHeight;
	}

	IW_PLUGIN_SAND_API SandWorld(size_t chunkWidth, size_t chunkHeight, double scale);
	IW_PLUGIN_SAND_API SandWorld(size_t screenSizeX, size_t screenSizeY, size_t numberOfChunksX, size_t numberOfChunksY, double scale);

	IW_PLUGIN_SAND_API Cell& GetCell(int x, int y, SandField field = SandField::CELL);

	IW_PLUGIN_SAND_API void SetCell(int x, int y, const Cell& cell);

	IW_PLUGIN_SAND_API void MoveCell(int x, int y, int xto, int yto);
	//IW_PLUGIN_SAND_API void PushCell(int x, int y, int xto, int yto);

	IW_PLUGIN_SAND_API bool InBounds (int x, int y);
	IW_PLUGIN_SAND_API bool IsEmpty  (int x, int y);
	IW_PLUGIN_SAND_API void KeepAlive(int x, int y);

	IW_PLUGIN_SAND_API std::pair<int, int> GetChunkLocation(int x, int y);

	inline std::pair<int, int> GetChunkLocation(SandChunk* chunk)
	{
		return GetChunkLocation(chunk->m_x, chunk->m_y);
	}

	IW_PLUGIN_SAND_API void RemoveEmptyChunks();

	IW_PLUGIN_SAND_API SandChunk* GetChunk(int x,  int y);
	IW_PLUGIN_SAND_API SandChunk* GetChunkL(std::pair<int, int> location);
	IW_PLUGIN_SAND_API SandChunk* GetChunkDirect(std::pair<int, int> location);

	IW_PLUGIN_SAND_API SandChunk* CreateChunk(std::pair<int, int> location);
private:
	inline void AddFieldToChunk(SandChunk* chunk, FieldConfig& field) {
		chunk->AddField(
			field.memory->alloc(FieldSize(field.cellSize)),
			field.hasLocks
				? new std::mutex[m_chunkWidth * m_chunkHeight /*FieldSize<std::mutex>()*/]//(std::mutex*)field.memory->alloc(FieldSize<std::mutex>())
				: nullptr,
			field.onSetCell
		);
	}
};

IW_PLUGIN_SAND_END
