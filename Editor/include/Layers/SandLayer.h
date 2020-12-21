#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>

#include <random>

#include "iw/graphics/ParticleSystem.h"
#include "iw/graphics/Font.h"

#include "iw/util/set/sparse_set.h"

//#include "iw/util/memory/pool_allocator.h"

struct Player {
	iw::vector3 Movement = 0;
	iw::vector2 pMousePos = 0, MousePos = 0;
	iw::vector2 FireButtons = 0;
	float FireTimeout = 0;

	float Velocity = 0;
};

struct Enemy2 {
	iw::vector2 Spot = 0;
	float FireTimeout = 0;
};

enum class CellProperties : char {
	MOVE_DOWN           = 0b00000001,
	MOVE_DOWN_SIDE      = 0b00000010,
	MOVE_SIDE           = 0b00000100,
	MOVE_FORWARD        = 0b00001000,
	HIT_LIKE_PROJECTILE = 0b00010000,
	HIT_LIKE_BEAM       = 0b00100000
};
inline CellProperties operator|(CellProperties a,CellProperties b){return static_cast<CellProperties>(iw::val(a)|iw::val(b));}
inline char           operator&(CellProperties a,CellProperties b){return iw::val(a)&iw::val(b);}

enum class CellType : char {
	EMPTY,

	SAND,
	WATER,
	ROCK,
	METAL,
	DEBRIS,

	LASER,
	eLASER,
	BULLET
};

struct Cell {
	CellType Type;
	CellProperties Props;

	iw::Color Color;

	float Life = 0; // Life until the cell will die, only some cells use this

	int dX = 0;    // Velocity
	int dY = -1;

	int TileId = 0; // Tile id, 0 means that it belongs to noone

	int LastUpdateTick = 0; // Used to check if the cell has been updated in the current tick

	float Speed() const {  // Manhattan distance of velocity
		return (dX > 0 ? dX : -dX)
			+ (dY > 0 ? dY : -dY);
	}

	static void SetDefault(CellType type, const Cell& cell) {
		m_defaults.emplace(type, cell);
	}

	static const Cell& GetDefault(CellType type) {
		return m_defaults.at(type);
	}
private:
	static std::unordered_map<CellType, Cell> m_defaults;
};

struct LockingCell {
	Cell Cell;
	std::mutex Mutex;

	LockingCell() = default;

	LockingCell(const LockingCell& copy)
		: Cell(copy.Cell)
		, Mutex()
	{}

	LockingCell& operator=(const LockingCell& copy) {
		Cell = copy.Cell;
		return *this;
	}
};

struct Tile {
	std::vector<iw::vector2> Locations;
	const int InitialLocationsSize = 0;
	int TileId = 0;

	Tile() = default;

	Tile(std::vector<iw::vector2> locations, int scale)
		: InitialLocationsSize(locations.size() * 4)
	{
		for (iw::vector2& v : locations) {
			for (int x = 0; x < abs(scale); x++)
			for (int y = 0; y < abs(scale); y++) {
				Locations.push_back(v * scale + iw::vector2(x, y) - scale);
			}
		}

		static int s_tileId = 1;
		TileId = s_tileId++;
	}
};

struct SandChunk {
private:
	LockingCell* m_cells;
	std::unordered_map<size_t, std::vector<size_t>> m_changes;
	std::atomic<int> m_filledCellCount;

public:
	int m_x;
	int m_y;
	int m_width; // should be private
	int m_height;

	int m_minX, m_minY,
	    m_maxX, m_maxY; // rect around things to update

	int m_lastUpdateTick;

	SandChunk() = default;

	SandChunk(
		LockingCell* cells,
		int x, int y,
		size_t width, size_t height,
		int currentTick)
		: m_cells(cells)
		, m_x(x)
		, m_y(y) 
		, m_width(width), m_height(height)
		, m_filledCellCount(0)
		, m_minX(m_width)
		, m_minY(m_height)
		, m_maxX(-1)
		, m_maxY(-1)
		, m_lastUpdateTick(currentTick)
	{}

	SandChunk(
		SandChunk&& move) noexcept
		: m_cells(move.m_cells)
		, m_filledCellCount(move.m_filledCellCount.load())
		, m_width (move.m_width)
		, m_height(move.m_height)
		, m_x(move.m_x)
		, m_y(move.m_y)
		, m_minX(move.m_minX)
		, m_minY(move.m_minY)
		, m_maxX(move.m_maxX)
		, m_maxY(move.m_maxY)
		, m_lastUpdateTick(move.m_lastUpdateTick)
	{}

	void Step(
		int currentTick)
	{
		ResetRect(currentTick);
	}

	void Reset() {
		m_filledCellCount = 0;
		m_lastUpdateTick = 0;

		LockingCell default;
		default.Cell = Cell::GetDefault(CellType::EMPTY);

		std::fill(m_cells, m_cells + m_width * m_height, default);
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
		return m_filledCellCount == 0;
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

		//std::unique_lock lock(out.Mutex);
		return currentTick == out.Cell.LastUpdateTick;
	}

	// new move cell idea
	//	Let cells move into the same cell, but then when commiting,
	//	pick a random one to actually move there

	void MoveCell(
		int x,   int y,
		int xto, int yto)
	{
		size_t index   = GetIndex(x,   y);
		size_t indexto = GetIndex(xto, yto);

		auto itr = m_changes.find(indexto);
		if (itr == m_changes.end()) {
			m_changes.emplace(indexto, std::vector<size_t> { index });
		}

		else {
			itr->second.push_back(index);
		}
	}

	void CommitMovedCells(
		int currentTick)
	{
		if (m_changes.size() == 0) return;

		for (auto& [indexto, possible] : m_changes) {
			size_t index = possible.at(iw::randi(possible.size() - 1));

			m_cells[indexto].Cell = m_cells[index].Cell; // does this conflict anything?
			m_cells[index].Cell = Cell::GetDefault(CellType::EMPTY); // this cant be here pass in arg

			m_cells[indexto].Cell.LastUpdateTick = currentTick; // set current tick

			UpdateRect(indexto % m_width, indexto / m_width);
		}

		m_changes.clear();

		m_lastUpdateTick = currentTick;
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
			--m_filledCellCount;
		}
		
		else if (cell.Type != CellType::EMPTY
			 && pcellType == CellType::EMPTY)
		{
			++m_filledCellCount;
		}

		LockingCell& assign = m_cells[GetIndex(x, y)];

		//std::unique_lock lock(assign.Mutex);
		assign.Cell = cell;
		assign.Cell.LastUpdateTick = currentTick;

		UpdateRect(x, y);

		m_lastUpdateTick = currentTick;
	}

	CellType GetCellType(
		int x, int y)
	{
		LockingCell& out = m_cells[GetIndex(x, y)];

		//std::unique_lock lock(out.Mutex);
		return out.Cell.Type;
	}

	Cell GetCell(
		int x, int y)
	{
		LockingCell& out = m_cells[GetIndex(x, y)];

		//std::unique_lock lock(out.Mutex);
		return out.Cell;
	}

	const Cell& GetCellUnsafe(
		int x, int y)
	{
		return m_cells[GetIndex(x, y)].Cell;
	}
private:
	void UpdateRect(
		int x, int y)
	{
		if (x <= m_minX) {
			m_minX = iw::clamp(x - 1, 0, m_width);
		}

		if (y <= m_minY) {
			m_minY = iw::clamp(y - 1, 0, m_height);
		}

		if (x >= m_maxX) {
			m_maxX = iw::clamp(x + 1, 0, m_width);
		}

		if (y >= m_maxY) {
			m_maxY = iw::clamp(y + 1, 0, m_height);
		}
	}

	void ResetRect(
		int currentTick)
	{
		//if (currentTick % 10 == 0) {
		//	m_minX = iw::clamp(m_minX + 1, 0, m_width);
		//	m_minY = iw::clamp(m_minY + 1, 0, m_height);
		//	m_maxX = iw::clamp(m_maxX - 1, 0, m_width);
		//	m_maxY = iw::clamp(m_maxY - 1, 0, m_height);
		//}

		if(IsEmpty()) {
			m_minX = m_width;
			m_minY = m_height;
			m_maxX = -1;
			m_maxY = -1;
		}
	}
};

struct SandWorld {
private:
	bool m_fixedChunks;
	std::mutex m_chunksMutex;
	iw::pool_allocator m_chunkMem;

public:
	iw::sparse_set<int, SandChunk*> m_chunks;

	//std::unordered_map<int, SandChunk> m_chunks;  // all should be private
	const float m_scale;
	const int m_chunkWidth;
	const int m_chunkHeight;
	int m_currentTick = 0;

	SandWorld(
		int chunkWidth,
		int chunkHeight,
		float scale)
		: m_chunkWidth (chunkWidth  / scale)
		, m_chunkHeight(chunkHeight / scale)
		, m_scale(scale)
		, m_fixedChunks(false)
		, m_chunkMem((sizeof(LockingCell) * m_chunkWidth * m_chunkHeight
				  + sizeof(SandChunk)) * 16)
	{}

	SandWorld(
		int width,
		int height,
		int chunksX,
		int chunksY,
		float scale)
		: m_chunkWidth (width  / scale / chunksX)
		, m_chunkHeight(height / scale / chunksY)
		, m_scale(scale)
		, m_fixedChunks(true)
		, m_chunkMem((sizeof(LockingCell) * m_chunkWidth * m_chunkHeight
				  + sizeof(SandChunk)) * 16)
	{
		for (int x = -chunksX/2; x < chunksX/2; x++)
		for (int y = -chunksY/2; y < chunksY/2; y++) {
			GetChunkOrMakeNew(x, y);
		}
	}

	void Step() {
		m_currentTick++;

		std::vector<int> toRemove;

		for (auto itr = m_chunks.begin(); itr != m_chunks.end(); itr++) {
			if (m_currentTick - (*itr)->m_lastUpdateTick > 1 / iw::DeltaTime() * 5) {
				toRemove.push_back(itr.sparse_index());
			}

			else {
				(*itr)->Step(m_currentTick);
			}
		}

		for (int index : toRemove) {
			m_chunks.erase(index);
		}
	}

	void Reset() {
		for (SandChunk* chunk : m_chunks) {
			chunk->Reset();
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

	SandChunk* GetChunk(
		int chunkX, int chunkY)
	{
		auto itr = m_chunks.find(GetChunkIndex(chunkX, chunkY));
		if (itr != m_chunks.end()) {
			return *itr;																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																			itr;
		}

		return nullptr;
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
			SandChunk* chunk = GetChunkOrMakeNew(cx, cy);
			if (chunk) {
				visible.push_back(chunk);
			}
		}

		return visible;
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

		if (x < 0 && !inclusive) {
			x += m_chunkWidth;
			chunkX--;
		}

		if (y < 0 && !inclusive) {
			y += m_chunkHeight;
			chunkY--;
		}

		return {chunkX, chunkY};
	}

	SandChunk* GetChunkAndMapCoords(
		int& x, int& y)
	{
		auto [chunkX, chunkY] = GetChunkCoordsAndIntraXY(x, y);
		return GetChunkOrMakeNew(chunkX, chunkY);
	}

	SandChunk* GetChunkOrMakeNew(
		int chunkX, int chunkY)
	{
		if (abs(chunkX) > 100 || abs(chunkY) > 100) {
			return nullptr;
		}

		if (m_fixedChunks) {
			return GetChunk(chunkX, chunkY);
		}

		else {
			std::unique_lock lock(m_chunksMutex); // could also queue new chunks
			auto itr = m_chunks.find(GetChunkIndex(chunkX, chunkY));
			if (itr != m_chunks.end()) {
				return *itr;
			}

			return m_chunks.emplace(
					GetChunkIndex(chunkX, chunkY),
					new SandChunk(
						m_chunkMem.alloc<SandChunk>(),
							m_chunkWidth * chunkX, m_chunkHeight * chunkY,
							m_chunkWidth,		   m_chunkHeight,
							m_currentTick)
				);
		}
	}

	int GetChunkIndex(int chunkX, int chunkY) {
		return (100+chunkX) + (100+chunkY) * 100; // 1000 = max chunk y pos, seems bad

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

	void MoveCell(
		int x,   int y,
		int xto, int yto)
	{
		if (m_chunk.InBounds(xto, yto)) {
			m_chunk.MoveCell(x, y, xto, yto);
		}

		else {
			m_world.SetCell(xto + m_chunk.m_x, yto + m_chunk.m_y, m_chunk.GetCell(x, y));
			m_chunk.SetCell(x, y, Cell::GetDefault(CellType::EMPTY), m_world.m_currentTick);
		}
	}

	void SetCell(
		int x, int y,
		const Cell& cell)
	{
		if (m_chunk.InBounds(x, y)) {
			m_chunk.SetCell(x, y, cell, m_world.m_currentTick);
		}

		else {
			m_world.SetCell(x + m_chunk.m_x, y + m_chunk.m_y, cell);
		}
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

	// maybe these ones are good to have in parent?

	bool MoveDown(
		int x, int y,
		const Cell& cell,
		const Cell& replace);

	bool MoveDownSide(
		int x, int y,
		const Cell& cell,
		const Cell& replace);

	bool MoveSide(
		int x, int y,
		const Cell& cell,
		const Cell& replace);

	bool MoveForward(
		int x, int y,
		const Cell& cell,
		const Cell& replace,
		bool& hit,
		int& hitx, int& hity);

	/////////////////////////////////////////////
	// move to child class

	void HitLikeProj(
		int x,  int y,
		int lx, int ly,
		const Cell& cell);

	void HitLikeBeam(
		int x,  int y,
		int lx, int ly,
		const Cell& cell);
};

namespace iw {
	class SandLayer
		: public Layer
	{
	private:
		ref<Texture> m_sandTexture; // shorter access than through mesh->material->gettexture
		iw::Mesh m_sandScreen;

		iw::StaticPS m_stars;

		iw::ref<iw::Font> m_font;
		Mesh m_textMesh;

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
			const Cell& projectile,
			int whoFiredId)
		{
			iw::vector2 direction = (target - position).normalized();
			iw::vector2 normal = vector2(-direction.y, direction.x);

			iw::vector2 point = position + direction * 25 + normal * iw::randf() * 15;

			if (	   world.InBounds(point.x, point.y)
				&& world.GetCell (point.x, point.y).Type != CellType::EMPTY)
			{
				return;
			}

			float div = 2;
			while (abs((direction / div).major()) > 1) {
				direction /= div;
				div = iw::clamp(div - iw::randf(), 1.2f, 2.f);
			}

			Cell cell = projectile;
			cell.dX = direction.x * speed;
			cell.dY = direction.y * speed;
			cell.TileId = whoFiredId;

			world.SetCell(point.x, point.y, cell);
		}
	};

	std::vector<iw::vector2> FillLine(
		int x, int y,
		int x2, int y2);
}
