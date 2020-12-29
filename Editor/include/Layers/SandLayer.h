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

struct Asteroid {
	iw::vector2 Velocity;
};


// Block coords    0, 0, width, height

// World coords    -inf, -inf -> inf, inf

//std::pair<uint16_t, uint16_t> GetBlockCoord(
//	int32_t worldX, int32_t worldY)
//{
//	uint32_t x = worldX / 1;
//	return {worldX, worldY};
//}
//
//uint32_t GetChunkIndex(
//	int32_t worldX, int32_t worldY)
//{
//	uint16_t a;
//	return worldX;
//}

// World -> map of blocks


// Worker world -> mapped coords

// Bit field of properties

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

// Hard type of cell for special situations

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

	float pX =  0;  // 'Position'
	float pY =  0;
	float dX =  0;  // Velocity
	float dY = -1;

	int TileId = 0; // Tile id, 0 means that it belongs to noone

	int LastUpdateTick = 0; // Used to check if the cell has been updated in the current tick

	bool Gravitised = false; // If this cell should react to gravity

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
				Locations.push_back(v * scale + iw::vector2(x, y) - scale*2);
			}
		}

		static int s_tileId = 1;
		TileId = s_tileId++;
	}
};

// World 

struct SandChunk {
private:
	std::vector<std::pair<size_t, size_t>> m_changes; // destination, source
	std::atomic<uint32_t> m_filledCellCount;

public:
	Cell* m_cells; // all should be private

	const  int32_t m_x;
	const  int32_t m_y;
	const uint16_t m_width;
	const uint16_t m_height;

	uint16_t m_minX, m_minY,
		    m_maxX, m_maxY; // rect around things to update

	int64_t m_lastUpdateTick;

	SandChunk() = default;

	SandChunk(
		Cell* cells,
		int32_t x, int32_t y,
		uint16_t width, uint16_t height,
		int64_t currentTick)
		: m_cells(cells)
		, m_x(x)
		, m_y(y) 
		, m_width(width)
		, m_height(height)
		, m_lastUpdateTick(currentTick)
		, m_filledCellCount(0)
	{
		ResetRect();
	}

	SandChunk& operator=(
		const SandChunk& copy) 
	{
		m_cells		   = copy.m_cells;
		m_lastUpdateTick  = copy.m_lastUpdateTick;
		m_filledCellCount = copy.m_filledCellCount.load();

		ResetRect();

		return *this;
	}

	// unsafe
	void Reset() {
		m_filledCellCount = 0;
		m_lastUpdateTick = 0;

		ResetRect();

		memset(m_cells, 0, sizeof(Cell) * m_width * m_height);
	}

	// unsafe
	void SetFullRect() {
		UpdateRect(0, 0);
		UpdateRect(m_width, m_height);
	}

	// safe
	bool InBounds(
		uint16_t chunkX, uint16_t chunkY)
	{
		return  0 <= chunkX && chunkX < m_width
			&& 0 <= chunkY && chunkY < m_height;
	}

	// safe
	bool IsEmpty() {
		return m_filledCellCount == 0;
	}

	// unsafe
	bool IsEmpty(
		uint16_t chunkX, uint16_t chunkY)
	{
		return GetCellType(chunkX, chunkY) == CellType::EMPTY;
	}

	// unsafe, but should be safe I think
	bool CellAlreadyUpdated(
		uint16_t chunkX, uint16_t chunkY)
	{
		return GetCellUnsafe(chunkX, chunkY).LastUpdateTick == m_lastUpdateTick;
	}

	// unsafe
	CellType GetCellType(
		uint16_t x, uint16_t y)
	{
		return GetCellUnsafe(x, y).Type;
	}

	// unsafe
	Cell GetCell(
		uint16_t x, uint16_t y)
	{
		return GetCellUnsafe(x, y);
	}

	// unsafe, might become default
	const Cell& GetCellUnsafe(
		uint16_t x, uint16_t y)
	{
		return m_cells[GetIndex(x, y)];
	}
	
	// unsafe
	void SetCell(
		uint16_t chunkX, uint16_t chunkY,
		const Cell& cell,
		int64_t currentTick)
	{
		size_t index = GetIndex(chunkX, chunkY);

		SetCellData(index, cell, currentTick);

		m_changes.emplace_back(index, index); // for rect update
		m_lastUpdateTick = currentTick;
	}

	// unsafe
	void MoveCell(
		uint16_t chunkX,   uint16_t chunkY,
		uint16_t toChunkX, uint16_t toChunkY)
	{
		m_changes.emplace_back(
			GetIndex(toChunkX, toChunkY),
			GetIndex(chunkX, chunkY)
		);
	}

	// unsafe
	void CommitMovedCells(
		int64_t currentTick)
	{
		m_lastUpdateTick = currentTick;

		ResetRect();

		// remove moves that have their destinations filled
		//	also do special update for 'set' cells (dest == source)
		for (size_t i = 0; i < m_changes.size(); i++) {
			if (m_cells[m_changes[i].first].Type != CellType::EMPTY) {
				if (m_changes[i].first == m_changes[i].second) {
					UpdateRect(m_changes[i].first % m_width,
							 m_changes[i].first / m_width);
				}

				m_changes[i] = m_changes.back(); m_changes.pop_back();
				i--;
			}
		}

		// sort by destination

		std::sort(m_changes.begin(), m_changes.end(),
			[](auto& a, auto& b) { return a.first < b.first; }
		);

		// pick random source for each destination

		size_t ip = 0;

		for (size_t i = 0; i < m_changes.size(); i++) {
			if (    i + 1 == m_changes.size()
				|| m_changes[i + 1].first != m_changes[i].first)
			{
				size_t rand = ip + iw::randi(i - ip);

				MoveCellData(m_changes[rand], currentTick);

				ip = i + 1;
			}
		}

		m_changes.clear();
	}
private:
	uint32_t GetIndex(
		uint16_t chunkX, uint16_t chunkY)
	{
		return chunkX + chunkY * m_width;
	}

	void UpdateRect(
		uint16_t x, uint16_t y)
	{
		if (x <= m_minX) {
			m_minX = iw::clamp<uint16_t>(x - 2, 0, m_width);
		}

		if (y <= m_minY) {
			m_minY = iw::clamp<uint16_t>(y - 2, 0, m_height);
		}

		if (x >= m_maxX) {
			m_maxX = iw::clamp<uint16_t>(x + 2, 0, m_width);
		}

		if (y >= m_maxY) {
			m_maxY = iw::clamp<uint16_t>(y + 2, 0, m_height);
		}
	}

	void ResetRect() {
		m_minX = m_width;
		m_minY = m_height;
		m_maxX = -1;
		m_maxY = -1;
	}

	void MoveCellData(
		std::pair<uint16_t, uint16_t> change,
		uint64_t currentTick)
	{
		uint16_t dest = change.first;
		uint16_t src  = change.second;

		Cell& from = m_cells[src];

		SetCellData(dest, from, currentTick);

		from = Cell::GetDefault(CellType::EMPTY);

		UpdateRect(src  % m_width, src  / m_width);
		UpdateRect(dest % m_width, dest / m_width);
	}

	void SetCellData(
		uint32_t index,
		const Cell& cell,
		uint64_t currentTick)
	{
		Cell& dest = m_cells[index];

		if (    cell.Type == CellType::EMPTY
			&& dest.Type != CellType::EMPTY)
		{
			--m_filledCellCount;
		}
		
		else if (    cell.Type  != CellType::EMPTY
				&& dest.Type == CellType::EMPTY)
		{
			++m_filledCellCount;
		}

		dest = cell;
		dest.LastUpdateTick = currentTick;
	}
};

struct SandWorld {
public:
	iw::sparse_set<uint32_t, SandChunk*> m_chunks;

	const double m_scale; // all should be private
	const uint16_t m_chunkWidth;
	const uint16_t m_chunkHeight;
	const size_t   m_chunkCellSizeInBytes;
	uint64_t m_currentTick = 0;
private:
	bool m_fixedChunks;
	std::mutex m_chunksMutex;
	iw::pool_allocator m_chunkMem;

public:
	SandWorld(
		uint16_t chunkWidth,
		uint16_t chunkHeight,
		double scale)
		: m_chunkWidth (chunkWidth  / scale)
		, m_chunkHeight(chunkHeight / scale)
		, m_scale(scale)
		, m_chunkCellSizeInBytes(sizeof(Cell) * m_chunkWidth * m_chunkHeight)
		, m_chunkMem(m_chunkCellSizeInBytes * 16)
		, m_fixedChunks(false)
	{}

	SandWorld(
		uint32_t width,
		uint32_t height,
		uint32_t chunksX,
		uint32_t chunksY,
		double scale)
		: m_chunkWidth (width  / scale / chunksX)
		, m_chunkHeight(height / scale / chunksY)
		, m_scale(scale)
		, m_chunkCellSizeInBytes(sizeof(Cell) * m_chunkWidth * m_chunkHeight)
		, m_chunkMem(m_chunkCellSizeInBytes * 16)
		, m_fixedChunks(true)
	{
		for (int x = -chunksX/2; x < chunksX/2; x++)
		for (int y = -chunksY/2; y < chunksY/2; y++) {
			GetChunkOrMakeNew(x, y);
		}
	}

	void Reset() {
		for (SandChunk* chunk : m_chunks) {
			chunk->Reset();
		}
	}

	void Step() {
		m_currentTick++;

		std::vector<uint32_t> chunksToRemove;

		for (auto itr = m_chunks.begin(); itr != m_chunks.end(); itr++) {
			if (    (*itr)->IsEmpty()
				&& m_currentTick - (*itr)->m_lastUpdateTick > 1 / iw::DeltaTime() * 5) // bad conditions have this be a callback or something
			{
				chunksToRemove.push_back(itr.sparse_index());
			}
		}

		for (uint32_t index : chunksToRemove) {
			m_chunkMem.free(m_chunks.at(index)->m_cells, m_chunkCellSizeInBytes);
			m_chunks.erase(index);
		}
	}

	bool InBounds(
		int32_t worldX, int32_t worldY)
	{
		if (SandChunk* chunk = GetChunkAndMapCoords(worldX, worldY)) {
			return chunk->InBounds(worldX, worldY);
		}

		return false;
	}

	bool IsEmpty(
		int32_t worldX, int32_t worldY)
	{
		if (SandChunk* chunk = GetChunkAndMapCoords(worldX, worldY)) {
			return chunk->IsEmpty(worldX, worldY);
		}

		return false;
	}

	bool CellAlreadyUpdated(
		int32_t worldX, int32_t worldY)
	{
		return GetChunkAndMapCoords(worldX, worldY)
			->CellAlreadyUpdated(worldX, worldY);
	}

	Cell GetCell(
		int32_t worldX, int32_t worldY)
	{
		return GetChunkAndMapCoords(worldX, worldY)
			->GetCell(worldX, worldY);
	}

	void SetCell(
		int32_t worldX, int32_t worldY,
		const Cell& cell)
	{
		return GetChunkAndMapCoords(worldX, worldY)
			->SetCell(worldX, worldY, cell, m_currentTick);
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

	std::pair<uint16_t, uint16_t> GetChunkCoords(
		int32_t worldX, int32_t worldY)
	{
		int16_t chunkX = worldX % m_chunkWidth;
		int16_t chunkY = worldY % m_chunkHeight;

		if (worldX < -1) chunkX += m_chunkWidth; // should prolly be 0, seem below
		if (worldY < -1) chunkY += m_chunkHeight;

		return { chunkX, chunkY };
	}

	std::pair<float, float> GetChunkCoordsPerfect(
		float x, float y)
	{
		x = fmod(x, m_chunkWidth);
		y = fmod(y, m_chunkHeight);

		if (x < -1) x += m_chunkWidth;    // -1 not 0 because anything 1 < x < -1 rounds to 0 w/ ints, naa this seems weird
		if (y < -1) y += m_chunkHeight;

		return {x, y};
	}
private: 
	SandChunk* GetChunkAndMapCoords(
		int& x, int& y)
	{
		auto [chunkX, chunkY] = GetChunkCoordsAndIntraXY(x, y);
		return GetChunkOrMakeNew(chunkX, chunkY);
	}

	SandChunk* GetChunkOrMakeNew(
		int chunkX, int chunkY)
	{
		if (abs(chunkX) > 500 || abs(chunkY) > 500) {
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
							m_chunkMem.alloc<Cell>(m_chunkCellSizeInBytes),
							m_chunkWidth * chunkX, m_chunkHeight * chunkY,
							m_chunkWidth,		   m_chunkHeight,
							m_currentTick)
				);
		}
	}

	int GetChunkIndex(int chunkX, int chunkY) { // find a way to tightly fit this
		return (500+chunkX) + (500 +chunkY) * 1000; // 1000 = max chunk y pos, seems bad

		//return size_t(chunkX) | size_t(chunkY) << (sizeof(size_t)*8/2-1); // find better hash
	}
};

class SandWorker {
private:
	SandWorld& m_world;
	SandChunk& m_chunk;

public:
	SandWorker(
		SandWorld& world,
		SandChunk& chunk)
		: m_world(world)
		, m_chunk(chunk)
	{}

	bool CellAlreadyUpdated(
		int worldX, int worldY)
	{
		auto[chunkX, chunkY] = m_world.GetChunkCoords(worldX, worldY);

		if (m_chunk.InBounds(chunkX, chunkY)) {
			return m_chunk.CellAlreadyUpdated(chunkX, chunkY);
		}

		return m_world.CellAlreadyUpdated(worldX, worldY);
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

			//direction = (target - point).normalized();

			if (	   world.InBounds(point.x, point.y)
				&& world.GetCell (point.x, point.y).Type != CellType::EMPTY)
			{
				return;
			}

			float div = 1.7f;
			while (abs((direction / div).major()) > 1) {
				direction /= div;
			}

			auto [px, py] = world.GetIntraChunkCoords(point.x, point.y);

			Cell cell = projectile;
			cell.TileId = whoFiredId;
			cell.pX = px;
			cell.pY = py;
			cell.dX = direction.x * speed;
			cell.dY = direction.y * speed;

			world.SetCell(point.x, point.y, cell);
		}
	};

	std::vector<std::pair<int, int>> FillLine(
		int x, int y,
		int x2, int y2);
}
