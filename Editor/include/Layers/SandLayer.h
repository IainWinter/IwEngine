#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include <vector>
#include <unordered_map>

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
	iw::vector2 Direction = -iw::vector2::unit_y;

	bool Gravitised = false;

	int TileId = 0; // 0 = not part of tile
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
	}

	size_t GetIndex(
		int x, int y)
	{
		return x + y * m_width;
	}

	bool InBounds(
		int x, int y, bool inWorldCoords = false)
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
		bool bothBuffers = false)
	{
		bool isEmpty = GetCell(x, y).Type == CellType::EMPTY;

		if (bothBuffers && isEmpty) {
			return GetCell(x, y, true).Type == CellType::EMPTY;
		}

		return isEmpty;
	}

	Cell* SetCell(
		int x, int y,
		const Cell& cell) // could be ref
	{
		size_t index = GetIndex(x, y);
		return &m_additions.emplace(index, cell).first->second;
	}

	Cell& GetCell(
		int x, int y,
		bool bothBuffers = false)
	{
		size_t index = GetIndex(x, y);

		Cell* cell = &Cells[index];

		if (cell->Type == CellType::EMPTY && bothBuffers) {
			auto itr = m_additions.find(index);
			if (itr != m_additions.end()) {
				cell = &itr->second;
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
	SandChunk* m_cachedChunk;

	SandWorld(
		int width,
		int height,
		float scale)
		: m_chunkWidth (width / scale)
		, m_chunkHeight(height / scale)
		, m_scale(scale)
		, m_cachedChunk(nullptr)
	{
		for (size_t i  = 0; i  <= 10;  i++)
			m_chunks.emplace(iw::vector2(0, i), SandChunk(0, i * m_chunkWidth, m_chunkWidth, m_chunkHeight));
	}

	std::vector<std::pair<iw::vector2, SandChunk*>> GetVisibleChunks(
		int x,  int y,
		int x2, int y2)
	{
		std::vector<std::pair<iw::vector2, SandChunk*>> visible;

		auto [chunkX,  chunkY]  = GetChunkCoordsAndIntraXY(x,  y);
		auto [chunkX2, chunkY2] = GetChunkCoordsAndIntraXY(x2, y2);

		for(int cx = chunkX; cx </*=*/ chunkX2; cx++)
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

	size_t ChunkWidth() {
		return m_chunkWidth;
	}

	size_t ChunkHeight() {
		return m_chunkHeight;
	}

	bool InBounds(
		int x, int y)
	{
		SandChunk* chunk;
		UpdateCache(x, y, chunk);
		if (chunk) {
			return chunk->InBounds(x, y);
		}

		return false;
	}

	bool IsEmpty(
		int x, int y,
		bool bothBuffers = false)
	{
		SandChunk* chunk;
		UpdateCache(x, y, chunk);
		if (chunk) {
			return chunk->IsEmpty(x, y, bothBuffers);
		}

		return false;
	}

	Cell* SetCell(
		int x, int y,
		const Cell& cell)
	{
		SandChunk* chunk;
		UpdateCache(x, y, chunk);
		if (chunk) {
			return chunk->SetCell(x, y, cell);
		}

		return nullptr;
	}

	Cell& GetCell(
		int x, int y,
		bool bothBuffers = false)
	{
		return m_cachedChunk->GetCell(x, y, bothBuffers);
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

	bool UpdateCache(
		int& x, int& y, SandChunk*& chunk)
	{
		chunk = m_cachedChunk;

		auto [chunkX, chunkY] = GetChunkCoordsAndIntraXY(x, y);

		bool notCached = m_cachedChunk
			?       m_cachedChunk->m_x / m_chunkWidth  == chunkX
				&& m_cachedChunk->m_y / m_chunkHeight == chunkY
				&& m_cachedChunk->InBounds(x, y)
			: false;

		if (!notCached) {
			chunk = GetChunk(chunkX, chunkY);
			if (chunk) {
				m_cachedChunk = chunk;
			}
		}

		return notCached;
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

namespace iw {
	class SandLayer
		: public Layer
	{
	private:
		ref<Shader> shader;

		ref<RenderTarget> target;

		const Cell _EMPTY  = { CellType::EMPTY,  Color(0), 0, 0 };

		const Cell _SAND   = { CellType::SAND,   Color::From255(237, 201, 175), 1 };
		const Cell _WATER  = { CellType::WATER,  Color::From255(175, 201, 237)};
		const Cell _ROCK   = { CellType::ROCK,   Color::From255(201, 201, 201)};
		const Cell _METAL  = { CellType::METAL,  Color::From255(230, 230, 230), 10 };
		const Cell _DEBRIS = { CellType::DEBRIS, Color::From255(150, 150, 150), 1 };

		const Cell _LASER  = { CellType::LASER,  Color::From255(255,   0,   0), .06f };
		const Cell _BULLET = { CellType::BULLET, Color::From255(255,   255, 0), .02f };

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
				c->Direction = direction * speed;
				c->TileId = whoFiredId;
			}
		}

		std::vector<iw::vector2> FillLine(
			int x,  int y,
			int x2, int y2);
	};
}
