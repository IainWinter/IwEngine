#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include <vector>
#include <unordered_map>
#include <mutex>

struct Player {
	glm::vec3 Movement = glm::vec3(0.f);
	glm::vec2 pMousePos = glm::vec2(0.f), MousePos = glm::vec2(0.f);
	glm::vec2 FireButtons = glm::vec2(0.f);
	float FireTimeout = 0;
};

struct Enemy2 {
	glm::vec2 Spot = glm::vec2(0.f);
	float FireTimeout = 0;
};

struct v2i_hash {
	int operator() (const glm::vec2& v) const {
		return (int)v.x ^ (int)v.y;
	}
};

enum class CellType : short {
	EMPTY,

	SAND,
	WATER,
	ROCK,
	METAL,
	DEBRIS,

	LASER,
	BULLET,

	LIFE
};

struct Cell {
	CellType Type;
	iw::Color Color;

	float Life = 0;

	float x = 0,  y =  0,
		dx = 0, dy = -1;

	int TileId = 0; // 0 = not part of tile

	int LastUpdateTick = 0;

	float Speed() const {
		return sqrt(dx*dx + dy*dy);
	}
};

struct LockingCell {
	Cell Cell;
	std::mutex Mutex;

	LockingCell() = default;

	LockingCell(const LockingCell& copy)
		: Cell(copy.Cell)
		, Mutex()
	{}
};

struct Tile {
	std::vector<std::pair<glm::vec2, bool>> Locations;
	int TileId = 0;

	bool Destroy = false;

	Tile() = default;

	Tile(std::vector<glm::vec2> locations)
	{
		for (glm::vec2& v : locations) {
			Locations.emplace_back(v * 2.f + glm::vec2(1, 0) - 4.f, true);
			Locations.emplace_back(v * 2.f + glm::vec2(1, 1) - 4.f, true);
			Locations.emplace_back(v * 2.f + glm::vec2(0, 1) - 4.f, true);
			Locations.emplace_back(v * 2.f + glm::vec2(0, 0) - 4.f, true);
		}

		static int s_tileId = 1;
		TileId = s_tileId++;
	}
};

struct SandChunk {
private:
	std::vector<LockingCell> m_cells;
	std::atomic<int> m_emptyCellCount;

public:
	const int m_width; // should be private
	const int m_height;
	const int m_x;
	const int m_y;

	SandChunk() = default;

	SandChunk(int x, int y, int width, int height)
		: m_x(x), m_y(y)
		, m_width(width), m_height(height)
		, m_emptyCellCount(0)
	{
		m_cells.resize(m_width * m_height);
	}

	SandChunk(SandChunk&& move) noexcept
		: m_cells(move.m_cells)
		, m_emptyCellCount(move.m_emptyCellCount.load())
		, m_width (move.m_width)
		, m_height(move.m_height)
		, m_x     (move.m_x)
		, m_y     (move.m_y)
	{}

	void Reset() {
		m_emptyCellCount = 0;

		size_t size = m_cells.size();
		m_cells.clear();
		m_cells.resize(size);
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

	bool IsEmpty() {
		return m_emptyCellCount == 0;
	}

	bool IsEmpty(
		int x, int y)
	{
		return GetCellType(x, y) == CellType::EMPTY;
	}

	bool CellAlreadyUpdated(
		int x, int y,
		int currentTick)
	{
		LockingCell& out = m_cells[GetIndex(x, y)];

		std::unique_lock lock(out.Mutex);
		return currentTick == out.Cell.LastUpdateTick;
	}

	void SetCell(
		int x, int y,
		const Cell& cell,
		int currentTick)
	{
		CellType pcellType = GetCellType(x, y);

		if (    cell.Type == CellType::EMPTY
			&& pcellType != CellType::EMPTY)
		{
			++m_emptyCellCount;
		}
		
		else if (cell.Type != CellType::EMPTY
			 && pcellType == CellType::EMPTY)
		{
			--m_emptyCellCount;
		}

		LockingCell& assign = m_cells[GetIndex(x, y)];

		std::unique_lock lock(assign.Mutex);
		assign.Cell = cell;
		assign.Cell.LastUpdateTick = currentTick;

		assign.Cell.x = float(cell.x - int(cell.x)) + x;
		assign.Cell.y = float(cell.y - int(cell.y)) + y;
	}

	CellType GetCellType(
		int x, int y)
	{
		LockingCell& out = m_cells[GetIndex(x, y)];

		std::unique_lock lock(out.Mutex);
		return out.Cell.Type;
	}

	Cell GetCell(
		int x, int y)
	{
		LockingCell& out = m_cells[GetIndex(x, y)];

		std::unique_lock lock(out.Mutex);
		return out.Cell;
	}

	const Cell& GetCellUnsafe(
		int x, int y)
	{
		return m_cells[GetIndex(x, y)].Cell;
	}
};

struct SandWorld {
private:

public:
	std::unordered_map<int, SandChunk> m_chunks;  // all should be private
	float m_scale; 
	int m_chunkWidth;
	int m_chunkHeight;
	int m_currentTick = 0;

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
		for (int x = -chunksX/2; x < chunksX/2; x++)
		for (int y = -chunksY/2; y < chunksY/2; y++) {
			m_chunks.emplace(
				GetChunkIndex(x, y),
				SandChunk(
					m_chunkWidth * x, m_chunkHeight * y,
					m_chunkWidth,     m_chunkHeight)
			);
		}
	}

	std::vector<SandChunk*> GetVisibleChunks(
		int x,  int y,
		int x2, int y2)
	{
		std::vector<SandChunk*> visible;

		auto [chunkX,  chunkY]  = GetChunkCoordsAndIntraXY(x,  y);
		auto [chunkX2, chunkY2] = GetChunkCoordsAndIntraXY(x2, y2, true);

		for(int cx = chunkX; cx < chunkX2; cx++)
		for(int cy = chunkY; cy < chunkY2; cy++) {
			SandChunk* chunk = GetChunk(cx, cy);
			if (chunk) {
				visible.push_back(chunk);
			}
		}

		return visible;
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

	bool CellAlreadyUpdated(
		int x, int y)
	{
		if (SandChunk* chunk = GetChunkAndMapCoords(x, y)) {
			return chunk->InBounds(x, y, m_currentTick);
		}

		return false;
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
		int x, int y)
	{
		if (SandChunk* chunk = GetChunkAndMapCoords(x, y)) {
			return chunk->IsEmpty(x, y);
		}

		return false;
	}

	void SetCell(
		int x, int y,
		const Cell& cell)
	{
		if (SandChunk* chunk = GetChunkAndMapCoords(x, y)) {
			chunk->SetCell(x, y, cell, m_currentTick);
		}
	}

	Cell GetCell(
		int x, int y)
	{
		return GetChunkAndMapCoords(x, y)
			->GetCell(x, y);
	}
private:
	std::pair<int, int> GetChunkCoordsAndIntraXY(
		int& x, int& y,
		bool inclusive = false)
	{
		int chunkX, chunkY;

		if (inclusive) {
			chunkX = (int)ceil(float(x) / m_chunkWidth);//(x + m_chunkWidth - 1) / m_chunkWidth;
			chunkY = (int)ceil(float(y) / m_chunkHeight);//(y + m_chunkHeight - 1) / m_chunkHeight;
		}

		else {
			chunkX = x / m_chunkWidth;
			chunkY = y / m_chunkHeight;
		}

		x = x % m_chunkWidth;
		y = y % m_chunkHeight;

		if (x < 0) {
			x += m_chunkWidth;
			chunkX--;
		}

		if (y < 0) {
			y += m_chunkHeight;
			chunkY--;
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
		auto itr = m_chunks.find(GetChunkIndex(chunkX, chunkY));
		/*if (itr == m_chunks.end()) {
			itr = m_chunks.emplace(
					glm::vec2(chunkX, chunkY),
					SandChunk(m_chunkWidth * chunkX, m_chunkHeight * chunkY,
							m_chunkWidth,          m_chunkHeight)
				).first;
		}*/

		if (itr != m_chunks.end()) {
			return &itr->second;
		}

		return nullptr;//&itr->second;
	}

	int GetChunkIndex(int chunkX, int chunkY) {
		return chunkX + chunkY * 1000; // 1000 = max chunk y pos, seems bad

		//return size_t(chunkX) | size_t(chunkY) << (sizeof(size_t)*8/2-1); // find better hash
	}
};

class SandWorker {
private:
	SandWorld& m_world;
	SandChunk& m_chunk;

public:
	SandWorker(SandWorld& world, SandChunk& chunk) : m_world(world), m_chunk(chunk) {}

	bool CellAlreadyUpdated(
		int x, int y)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.CellAlreadyUpdated(x, y, m_world.m_currentTick);
		}

		return m_world.CellAlreadyUpdated(x, y);
	}

	bool InBounds(
		int x, int y)
	{
		if (m_chunk.InBounds(x, y)) {
			return true;
		}

		return m_world.InBounds(x + m_chunk.m_x, y + m_chunk.m_y);
	}

	bool IsEmpty(
		int x, int y)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.IsEmpty(x, y);
		}

		return m_world.IsEmpty(x + m_chunk.m_x, y + m_chunk.m_y);
	}

	void SetCell(
		int x, int y,
		const Cell& cell)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.SetCell(x, y, cell, m_world.m_currentTick);
		}

		return m_world.SetCell(x + m_chunk.m_x, y + m_chunk.m_y, cell);
	}

	Cell GetCell(
		int x, int y)
	{
		if (m_chunk.InBounds(x, y)) {
			return m_chunk.GetCell(x, y);
		}

		return m_world.GetCell(x + m_chunk.m_x, y + m_chunk.m_y);
	}

	void UpdateChunk();

	 // move to child class

	void MoveLikeSand(
		int x, int y,
		const Cell& cell,
		const Cell& replacement);

	void MoveLikeWater(
		int x, int y,
		const Cell& cell,
		const Cell& replacement);

	std::pair<bool, glm::vec2> MoveForward(
		int x, int y,
		const Cell& cell,
		const Cell& replacement);

	void HitLikeBullet(
		int x, int y,
		const Cell& cell);

	void HitLikeLaser(
		int x, int y,
		const Cell& cell);
};

namespace iw {
	class SandLayer
		: public Layer
	{
	private:
		ref<Shader> shader;

		ref<RenderTarget> target;
		ref<Texture> texture;

		glm::vec2 pMousePos, mousePos;
		float fireTimeout = 0;

		iw::Entity player;

		glm::vec2 gravPos;

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
			glm::vec2 position,
			glm::vec2 target,
			float speed,
			const Cell& projectile,
			int whoFiredId)
		{
			glm::vec2 direction = glm::normalize(target - position);
			glm::vec2 point = position + direction * glm::vec2(8, 10) + glm::vec2(iw::randf(), iw::randf()) * 5.f;

			LOG_INFO << direction.x << ", " << direction.y;

			if (	   world.InBounds(point.x, point.y)
				&& world.GetCell (point.x, point.y).Type != CellType::EMPTY)
			{
				return;
			}

			//float div = 2;
			//while (abs(glm::compMax(direction / div)) > 1) {
			//	direction /= div;
			//	div = iw::clamp(div - iw::randf(), 1.2f, 2.f);
			//}

			Cell cell = projectile;
			cell.dx = direction.x * speed;
			cell.dy = direction.y * speed;
			cell.TileId = whoFiredId;

			world.SetCell(point.x, point.y, cell);
		}
	};

	std::vector<glm::ivec2> FillLineInGrid(
		int x,  int y,
		int x2, int y2);

	std::vector<glm::vec2> FillLine(
		float x,  float y,
		float x2, float y2);
}
