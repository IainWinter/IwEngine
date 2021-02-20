#pragma once

#include "Cell.h"
#include <array>
#include <vector>
#include <algorithm>
//#include <mutex>

class SandChunk {
private:
	std::vector<std::tuple<SandChunk*, size_t, size_t>> m_changes; // source chunk, source, destination
	//std::mutex m_filledCellCountMutex;
public:
	Cell* m_cells;

	const size_t m_width;
	const size_t m_height;
	const int m_x;
	const int m_y;

	size_t m_filledCellCount;

	int m_minX; // Dirty rect
	int m_minY;
	int m_maxX;
	int m_maxY;

public:
	SandChunk(
		Cell* cells,
		size_t width, size_t height,
		int x, int y)
		: m_cells(cells)
		, m_width(width)
		, m_height(height)
		, m_x(x * width)
		, m_y(y * height)

		, m_filledCellCount(0)

		, m_minX(m_width)
		, m_minY(m_height)
		, m_maxX(0)
		, m_maxY(0)
	{}

	// Getting cells

	Cell& GetCell(int x, int y) {
		return GetCell(GetIndex(x, y));
	}

	Cell& GetCell(size_t index) {
		KeepAlive(index % m_width, index / m_width);
		return m_cells[index];
	}

	// Setting & moving cells

	void SetCell(int x, int y, const Cell& cell) { SetCell(GetIndex(x, y), cell); }

	void SetCell(
		size_t index,
		const Cell& cell)
	{
		Cell& dest = m_cells[index];

		if (    dest.Type == CellType::EMPTY
			&& cell.Type != CellType::EMPTY) // Filling a cell
		{
			//std::unique_lock lock(m_filledCellCountMutex);
			m_filledCellCount++;
		}

		else if (dest.Type != CellType::EMPTY
			&&  cell.Type == CellType::EMPTY) // Removing a filled cell
		{
			//std::unique_lock lock(m_filledCellCountMutex);
			m_filledCellCount--;
		}

		dest = cell;

		KeepAlive(index % m_width, index / m_width);
	}

	void MoveCell(
		SandChunk* source,
		int x,   int y,
		int xto, int yto)
	{
		m_changes.emplace_back(
			source,
			source->GetIndex(x, y),
			GetIndex(xto, yto)
		);
	}

#define _DEST 2

	void CommitCells() {
		ResetRect();

		// remove moves that have their destinations filled

		for (size_t i = 0; i < m_changes.size(); i++) {
			const Cell& dest = m_cells[std::get<_DEST>(m_changes[i])];

			if (dest.Type != CellType::EMPTY) {
				m_changes[i] = m_changes.back(); m_changes.pop_back();
				i--;
			}
		}

		// sort by destination

		std::sort(m_changes.begin(), m_changes.end(),
			[](auto& a, auto& b) { return std::get<_DEST>(a) < std::get<_DEST>(b); }
		);

		// pick random source for each destination

		size_t iprev = 0;

		m_changes.emplace_back(nullptr, -1, -1); // to catch final move

		for (size_t i = 0; i < m_changes.size() - 1; i++) {
			if (std::get<_DEST>(m_changes[i + 1]) != std::get<_DEST>(m_changes[i])) {
				size_t rand = iprev + iw::randi(i - iprev);

				auto [chunk, src, dest] = m_changes[rand];

				       SetCell(dest, chunk->GetCell(src));
				chunk->SetCell(src,  Cell());

				iprev = i + 1;
			}
		}

		m_changes.clear();
	}

	// Helpers

	size_t GetIndex(int x, int y) {
		x -= m_x;
		y -= m_y;
		return x + y * m_width;
	}

	bool InBounds(int x, int y) {
		return  x >= m_x && x < int(m_x + m_width)
			&& y >= m_y && y < int(m_y + m_height);
	}

	bool IsEmpty (int x, int y) {
		return GetCell(x, y).Type == CellType::EMPTY;
	}

	void KeepAlive(int x, int y) {
		m_minX = iw::clamp<int>(min(x - 3, m_minX), 0, m_width);
		m_minY = iw::clamp<int>(min(y - 3, m_minY), 0, m_height);
		m_maxX = iw::clamp<int>(max(x + 3, m_maxX), 0, m_width);
		m_maxY = iw::clamp<int>(max(y + 3, m_maxY), 0, m_height);
	}

	void ResetRect() {
		m_minX = m_width;
		m_minY = m_height;
		m_maxX = -1;
		m_maxY = -1;
	}
};

class SandWorld {
public:
	const size_t m_chunkWidth;
	const size_t m_chunkHeight;
	const size_t m_chunkSizeInBytes;
	const double m_scale;

private:
	iw::pool_allocator m_cells;
	iw::pool_allocator m_chunks;

	std::array<std::vector<SandChunk*>, 4> m_chunkBatches;
	std::unordered_map<std::pair<int, int>, SandChunk*, iw::pair_hash> m_chunkLookup;

	std::mutex m_chunkMutex;

public:
	SandWorld(
		size_t chunkWidth,
		size_t chunkHeight,
		double scale)
		: m_chunkWidth (chunkWidth  / scale)
		, m_chunkHeight(chunkHeight / scale)
		, m_chunkSizeInBytes(sizeof(Cell) * m_chunkWidth * m_chunkHeight)
		, m_cells(m_chunkSizeInBytes * 16)
		, m_chunks(sizeof(SandChunk) * 16)
		, m_scale(scale)
	{}

	// Getting cells

	Cell& GetCell(
		int x, int y)
	{
		return GetChunk(x, y)->GetCell(x, y);
	}

	// Setting & moving cells

	void SetCell(
		int x, int y,
		const Cell& cell)
	{
		if (SandChunk* chunk = GetChunk(x, y)) {
			chunk->SetCell(x, y, cell);
		}
	}

	void MoveCell(
		int x,   int y,
		int xto, int yto)
	{
		if (SandChunk* src  = GetChunk(x, y))
		if (SandChunk* dest = GetChunk(xto, yto)) {
			dest->MoveCell(src, x, y, xto, yto);
		}
	}

	// Helpers

	bool InBounds(int x, int y) {
		if (SandChunk* chunk = GetChunk(x, y)) {
			return chunk->InBounds(x, y);
		}

		return false;
	}

	bool IsEmpty(int x, int y) {
		return  InBounds(x, y)
			&& GetChunk(x, y)->IsEmpty(x, y);
	}

	SandChunk* GetChunk(
		int x, int y)
	{
		auto location = GetChunkLocation(x, y);

		SandChunk* chunk = GetChunkDirect(location);
		if (!chunk) {
			chunk = CreateChunk(location);
		}

		return chunk;
	}

	SandChunk* GetChunkDirect(
		std::pair<int, int> location)
	{
		auto itr = m_chunkLookup.find(location);
		if (itr == m_chunkLookup.end()) {
			return nullptr;
		}

		return itr->second;
	}

	std::pair<int, int> GetChunkLocation(
		int x, int y)
	{
		return {
			floor(float(x) / m_chunkWidth),
			floor(float(y) / m_chunkHeight),
		};
	}

	std::array<std::vector<SandChunk*>, 4> GetChunkBatches() const {
		return m_chunkBatches;
	}

	void RemoveEmptyChunks() {
		std::vector<std::tuple<size_t, size_t, std::pair<int, int>>> remove;

		for (size_t b = 0; b < m_chunkBatches      .size(); b++)
		for (size_t i = 0; i < m_chunkBatches.at(b).size(); i++) {

			SandChunk* chunk = m_chunkBatches.at(b).at(i);

			if (chunk->m_filledCellCount == 0) {
				remove.emplace_back(b, i, GetChunkLocation(chunk->m_x, chunk->m_y));
			}
		}

		for (auto& [batch, index, location] : remove) {
			LOG_INFO << "Removing chunk " << location.first << " " << location.second;

			SandChunk*& chunk = m_chunkBatches.at(batch).at(index);

			m_cells .free(chunk->m_cells, m_chunkSizeInBytes);
			m_chunks.free(chunk);

			chunk = m_chunkBatches.at(batch).back();
			m_chunkLookup.erase(m_chunkLookup.find(location));
		}

		for (auto [batch, _, __] : remove) {
			m_chunkBatches.at(batch).pop_back();
		}
	}

private:
	SandChunk* CreateChunk(
		std::pair<int, int> location)
	{
		auto [lx, ly] = location;

		if(     lx < -100 || ly < -100
			|| lx >  100 || ly >  100) // could pass in a world limit to constructor
		{
			return nullptr;
		}

		SandChunk* chunk = m_chunks.alloc<SandChunk>();
		new (chunk) SandChunk(
			m_cells.alloc<Cell>(m_chunkSizeInBytes),
			m_chunkWidth, m_chunkHeight, lx, ly
		);

		int batch = int(lx % 2 == 0)
			     | int(ly % 2 == 0) << 1;

		m_chunkLookup.insert({ location, chunk });

		//{
			//std::unique_lock lock(m_chunkMutex);
			m_chunkBatches.at(batch).push_back(chunk);
		//}

		return chunk;
	}
};

class SandWorker {
protected:
	SandWorld& m_world;
	SandChunk* m_chunk;

public:
	SandWorker(
		SandWorld& world,
		SandChunk* chunk)
		: m_world(world)
		, m_chunk(chunk)
	{}

	// Updating cells

	void UpdateChunk() {
		for (int x = m_chunk->m_minX; x < m_chunk->m_maxX; x++)
		for (int y = m_chunk->m_minY; y < m_chunk->m_maxY; y++) {
			Cell& cell = m_chunk->GetCell(x + y * m_chunk->m_width);

			size_t px = x + m_chunk->m_x;
			size_t py = y + m_chunk->m_y;

			UpdateCell(px, py, cell);
		}
	}

	virtual void UpdateCell(
		int x, int y,
		Cell& cell) = 0;

	// Getting cells

	Cell& GetCell(
		int x, int y)
	{
		if (m_chunk->InBounds(x, y)) {
			return m_chunk->GetCell(x, y);
		}

		return m_world.GetCell(x, y);
	}

	// Setting & moving cells

	void SetCell(
		int x, int y,
		const Cell& cell)
	{
		if (m_chunk->InBounds(x, y)) {
			return m_chunk->SetCell(x, y, cell);
		}

		return m_world.SetCell(x, y, cell);
	}

	void MoveCell(
		int x,   int y,
		int xto, int yto)
	{
		if (m_chunk->InBounds(x,   y  ))
		if (m_chunk->InBounds(xto, yto)) {
			return m_chunk->MoveCell(m_chunk, x, y, xto, yto);
		}

		return m_world.MoveCell(x, y, xto, yto);
	}

	// Helpers

	bool InBounds(int x, int y) {
		return  m_chunk->InBounds(x, y)
			|| m_world .InBounds(x, y);
	}

	bool IsEmpty(int x, int y) {
		if (m_chunk->InBounds(x, y)) {
			return m_chunk->IsEmpty(x, y);
		}

		return m_world.IsEmpty(x, y);
	}
};
