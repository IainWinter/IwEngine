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

#include "Sand/Cell.h"

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

#include "Sand/SandChunk.h"

struct SandWorld {
public:
	iw::sparse_set<uint32_t, SandChunk*> m_chunks;

	const double m_scale;
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
		m_chunks.clear();
		m_chunkMem.reset();
	}

	void Step() {
		m_currentTick++;

		std::vector<uint32_t> chunksToRemove;

		for (auto itr = m_chunks.begin(); itr != m_chunks.end(); itr++) {
			if ((*itr)->IsAllEmpty()) {
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
			return chunk->GetCell(worldX, worldY).Type == CellType::EMPTY;
		}

		return false;
	}

	bool CellUpdated(
		int32_t worldX, int32_t worldY)
	{
		return GetChunkAndMapCoords(worldX, worldY)
			->GetCell(worldX, worldY).LastUpdateTick == m_currentTick;
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
							m_chunkWidth,		   m_chunkHeight
					)
			);
		}
	}

	int GetChunkIndex(int chunkX, int chunkY) { // find a way to tightly fit this
		return (500+chunkX) + (500 +chunkY) * 1000; // 1000 = max chunk y pos, seems bad

		//return size_t(chunkX) | size_t(chunkY) << (sizeof(size_t)*8/2-1); // find better hash
	}
};

#include "Sand/SandWorker.h"

class DefaultSandWorker : public SandWorker {
public:
	void UpdateChunk() override;

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

			if (!world.IsEmpty(point.x, point.y)) {
				return;
			}

			Cell cell = projectile;
			cell.TileId = whoFiredId;
			cell.pX = point.x;
			cell.pY = point.y;
			cell.dX = direction.x * speed;
			cell.dY = direction.y * speed;

			world.SetCell(point.x, point.y, cell);
		}
	};

	std::vector<std::pair<int, int>> FillLine(
		int x, int y,
		int x2, int y2);
}
