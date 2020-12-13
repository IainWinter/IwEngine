#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include <vector>
#include <unordered_map>
#include <mutex>

struct v2i_hash {
	int operator() (const iw::vector2& v) const {
		return (int)v.x ^ (int)v.y;
	}
};

enum class CellType {
	EMPTY,

	SAND,
	WATER,
	ROCK,
	METAL,
	DEBRIS,

	LASER,
	BULLET
};

struct Cell {
	CellType Type;
	iw::Color Color;

	float Life = 0;
	int dX = 0;
	int dY = -1; // Velocity

	bool Gravitised = false;

	int TileId = 0; // 0 = not part of tile

	float Speed() {
		return (dX > 0 ? dX : -dX)
			+ (dY > 0 ? dY : -dY); // approx
	}
};

struct Tile {
	std::vector<iw::vector2> Locations;
	const int InitialLocationsSize = 0;
	int TileId;

	Tile() = default;

	Tile(std::vector<iw::vector2> locations)
		: InitialLocationsSize(locations.size() * 4)
	{
		for (iw::vector2& v : locations) {
			Locations.push_back(v * 2 + iw::vector2(1, 0) - 4);
			Locations.push_back(v * 2 + iw::vector2(1, 1) - 4);
			Locations.push_back(v * 2 + iw::vector2(0, 1) - 4);
			Locations.push_back(v * 2 + iw::vector2(0, 0) - 4);
		}

		static int s_tileId = 1;
		TileId = s_tileId++;
	}
};

struct Player {
	iw::vector3 Movement = 0;
	iw::vector2 pMousePos = 0, MousePos = 0;
	iw::vector2 FireButtons = 0;
	float FireTimeout = 0;
};

struct Enemy2 {
	iw::vector2 Spot = 0;
	float FireTimeout = 0;
};

struct SandChunk {
	std::vector<Cell> Cells;
	std::unordered_map<size_t, Cell> m_additions;

	std::unordered_map<size_t, Cell> m_threadedAdditionQueue;

	size_t m_width;
	size_t m_height;
	int m_x;
	int m_y;

	SandChunk() = default;

	SandChunk(int x, int y, int width, int height)
		: m_x(x), m_y(y)
		, m_width(width), m_height(height)
	{
		Cells.resize(m_width * m_height);
	}

	void Reset() {
		size_t size = Cells.size();
		Cells.clear(); Cells.resize(size);
	}

	void CommitAdditions() {
		for (auto& [index, cell] : m_additions) {
			Cells[index] = cell;
		}

		m_additions.clear();

		for (auto& [index, cell] : m_threadedAdditionQueue) {
			Cells[index] = cell;
		}

		m_threadedAdditionQueue.clear();
	}

	size_t GetIndex(
		int x, int y)
	{
		return x + y * m_width;
	}

	bool InBounds(
		int x, int y,
		bool inWorldCoords = false)
	{
		if (inWorldCoords) {
			return  m_x <= x && x < m_x + m_width
				&& m_y <= y && y < m_y + m_height;
		}

		return  0 <= x && x < m_width
			&& 0 <= y && y < m_height;
	}

	bool IsEmpty(
		int x, int y,
		bool bothBuffers = false,
		bool threaded = false)
	{
		bool isEmpty = GetCell(x, y).Type == CellType::EMPTY;

		if (bothBuffers && isEmpty) {
			return GetCell(x, y, true, threaded).Type == CellType::EMPTY;
		}

		return isEmpty;
	}

	Cell* SetCell(
		int x, int y,
		const Cell& cell,
		bool threaded = false) // could be ref
	{
		size_t index = GetIndex(x, y);

		if (threaded) {
			return &m_threadedAdditionQueue.emplace(index, cell).first->second;
		}

		return &m_additions.emplace(index, cell).first->second;
	}

	Cell& GetCell(
		int x, int y,
		bool bothBuffers = false,
		bool threaded = false)
	{
		size_t index = GetIndex(x, y);

		Cell* cell = &Cells[index];

		if (cell->Type == CellType::EMPTY && bothBuffers) {
			if (threaded) {
				auto itr = m_threadedAdditionQueue.find(index);
				if (itr != m_threadedAdditionQueue.end()) {
					cell = &itr->second;
				}
			}
			else {
				auto itr = m_additions.find(index);
				if (itr != m_additions.end()) {
					cell = &itr->second;
				}
			}
		}

		return *cell;
	}
};

struct SandWorld {
	float m_scale;
	int m_chunkWidth;
	int m_chunkHeight;

	std::unordered_map<iw::vector2, SandChunk, v2i_hash> m_chunks;
	std::mutex m_additionMutex;

	SandWorld(
		int width,
		int height,
		int chunksX,
		int chunksY,
		float scale)
		: m_chunkWidth (width / scale / chunksX)
		, m_chunkHeight(height / scale / chunksY)
		, m_scale(scale)
	{
		for (size_t x  = 0; x  < chunksX; x++)
		for (size_t y  = 0; y  < chunksY; y++) {
			m_chunks.emplace(iw::vector2(x, y), SandChunk(x * m_chunkWidth, y * m_chunkHeight, m_chunkWidth, m_chunkHeight));
		}
	}

	std::vector<std::pair<iw::vector2, SandChunk*>> GetVisibleChunks(
		int x,  int y,
		int x2, int y2)
	{
		std::vector<std::pair<iw::vector2, SandChunk*>> visible;

		auto [chunkX,  chunkY]  = GetChunkCoordsAndIntraXY(x,  y);
		auto [chunkX2, chunkY2] = GetChunkCoordsAndIntraXY(x2, y2);

		for(int cx = chunkX; cx < chunkX2; cx++)
		for(int cy = chunkY; cy <  chunkY2; cy++) {
			SandChunk* chunk = GetChunk(cx, cy);
			if (chunk) {
				visible.emplace_back(iw::vector2(cx, cy), chunk);
			}
		}

		return visible;
	}

	void CommitAdditions() {
		for (auto& [_, chunk] : m_chunks) {
			chunk.CommitAdditions();
		}
	}

	void Reset() {
		for (auto& [_, chunk] : m_chunks) {
			chunk.Reset();
		}
	}

	size_t ChunkWidth() {
		return m_chunkWidth;
	}

	size_t ChunkHeight() {
		return m_chunkHeight;
	}

	bool InBounds(
		int x, int y)
	{
		if (SandChunk* chunk = GetChunkAndMapCoords(x, y)) {
			return chunk->InBounds(x, y);
		}

		return false;
	}

	bool IsEmpty(
		int x, int y,
		bool bothBuffers = false)
	{
		if (SandChunk* chunk = GetChunkAndMapCoords(x, y)) {
			if (bothBuffers) {
				std::unique_lock lock(m_additionMutex);
				return chunk->IsEmpty(x, y, bothBuffers, true);
			}

			return chunk->IsEmpty(x, y, bothBuffers);
		}

		return false;
	}

	Cell* SetCell(
		int x, int y,
		const Cell& cell)
	{
		if (SandChunk* chunk = GetChunkAndMapCoords(x, y)) {
			std::unique_lock lock(m_additionMutex);
			return chunk->SetCell(x, y, cell, true);
		}

		return nullptr;
	}

	Cell& GetCell(
		int x, int y,
		bool bothBuffers = false)
	{
		SandChunk* chunk = GetChunkAndMapCoords(x, y);
		if (bothBuffers) {
			std::unique_lock lock(m_additionMutex);
			return chunk->GetCell(x, y, bothBuffers, true);
		}

		return chunk->GetCell(x, y, bothBuffers);
	}
private:
	std::pair<int, int> GetChunkCoordsAndIntraXY(int& x, int& y) {
		int chunkX, chunkY;

		if (x >= 0) {
			chunkX = x / m_chunkWidth;
			x      = x % m_chunkWidth;
		}

		else {
			chunkX = (-m_chunkWidth + x) / m_chunkWidth;
			x      = ( m_chunkWidth + x) % m_chunkWidth;
		}

		if (y >= 0) {
			chunkY = y / m_chunkHeight;
			y      = y % m_chunkHeight;
		}

		else {
			chunkY = (-m_chunkHeight + y) / m_chunkHeight;
			y      = ( m_chunkHeight + y) % m_chunkHeight;
		}

		return {chunkX, chunkY};
	}

	SandChunk* GetChunkAndMapCoords(
		int& x, int& y)
	{
		auto [chunkX, chunkY] = GetChunkCoordsAndIntraXY(x, y);
		return GetChunk(chunkX, chunkY);
	}

	SandChunk* GetChunk(
		int chunkX, int chunkY)
	{
		auto itr = m_chunks.find(iw::vector2(chunkX, chunkY));
		if (itr != m_chunks.end()) {
			return &itr->second;
		}

		return nullptr;
	}
};

class SandWorker {
private:
	SandWorld& m_world;
	SandChunk& m_chunk;

public:
	SandWorker(SandWorld& world, SandChunk& chunk) : m_world(world), m_chunk(chunk) {}

	bool InBounds(
		int x, int y)
	{
		if (m_chunk.InBounds(x, y)) {
			return true;
		}

		return m_world.InBounds(x + m_chunk.m_x, y + m_chunk.m_y);
	}

	bool IsEmpty(
		int x, int y,
		bool bothBuffers = false)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.IsEmpty(x, y, bothBuffers);
		}

		return m_world.IsEmpty(x + m_chunk.m_x, y + m_chunk.m_y, bothBuffers);
	}

	Cell* SetCell(
		int x, int y,
		const Cell& cell)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.SetCell(x, y, cell);
		}

		return m_world.SetCell(x + m_chunk.m_x, y + m_chunk.m_y, cell);
	}

	Cell& GetCell(
		int x, int y,
		bool bothBuffers = false)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.GetCell(x, y, bothBuffers);
		}

		return m_world.GetCell(x + m_chunk.m_x, y + m_chunk.m_y, bothBuffers);
	}

	void UpdateChunk();

	 // move to child class

	void MoveLikeSand(
		int x, int y,
		Cell& cell,
		const Cell& replacement);

	void MoveLikeWater(
		int x, int y,
		const Cell& cell,
		const Cell& replacement);

	std::pair<bool, iw::vector2> MoveForward(
		int x, int y,
		Cell& cell,
		const Cell& replacement);

	void HitLikeBullet(
		int x, int y,
		Cell& cell);

	void HitLikeLaser(
		int x, int y,
		Cell& cell);
};

namespace iw {
	class SandLayer
		: public Layer
	{
	private:
		ref<Shader> shader;

		ref<RenderTarget> target;
		ref<Texture> texture;

		iw::vector2 pMousePos, mousePos;
		float fireTimeout = 0;

		iw::Entity player;

		iw::vector2 gravPos;

		float spawnEnemy = 0;

		bool reset = false;

		SandWorld world;

		float stepTimer = 0;

	public:
		SandLayer();

		int Initialize() override;
		void PostUpdate() override;

		bool On(MouseMovedEvent& e) override;
		bool On(MouseButtonEvent& e) override;
		bool On(KeyEvent& e) override;
	private:
		void Fire(
			vector2 position,
			vector2 target,
			float speed,
			const Cell& cell,
			int whoFiredId)
		{
			vector2 direction = (target - position).normalized();
			iw::vector2 point = position + direction * iw::vector2(5, 7) + vector2(iw::randf(), iw::randf()) * 5;

			if (	   world.InBounds(point.x, point.y)
				&& world.GetCell(point.x, point.y).Type != CellType::EMPTY)
			{
				return;
			}

			Cell* c = world.SetCell(point.x, point.y, cell);
			if (c) {
				c->dX = ceil(direction.x * speed);
				c->dY = ceil(direction.y * speed);
				c->TileId = whoFiredId;
			}
		}
	};

	std::vector<iw::vector2> FillLine(
		int x, int y,
		int x2, int y2);
}
