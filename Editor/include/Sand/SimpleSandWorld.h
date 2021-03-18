#pragma once

#include "Cell.h"
#include <array>
#include <vector>
#include <algorithm>
//#include <mutex>

#pragma once

#include "Cell.h"
#include <array>
#include <vector>
#include <algorithm>
//#include <mutex>

#include <concurrent_unordered_map.h>

class SandChunk {
public:
	Cell* m_cells;

	const size_t m_width, m_height;
	const int m_x, m_y;

	size_t m_filledCellCount;

	int m_minX, m_minY,
	    m_maxX, m_maxY; // Dirty rect

private:
	std::vector<std::tuple<SandChunk*, size_t, size_t>> m_changes; // source chunk, source, destination

	std::mutex m_filledCellCountMutex;
	std::mutex m_changesMutex;
	std::mutex m_workingRectMutex;

	int m_minXw, m_minYw,
	    m_maxXw, m_maxYw; // working dirty rect

public:
	SandChunk(
		size_t width, size_t height,
		int x, int y)
		: m_width(width)
		, m_height(height)
		, m_x(x * width)
		, m_y(y * height)

		, m_filledCellCount(0)
	{
		m_cells = new Cell[width * height];
		UpdateRect();
		UpdateRect();
	}

	~SandChunk() {
		delete[] m_cells;
	}

	// Getting cells

	Cell& GetCell(int x, int y) {
		return GetCell(GetIndex(x, y));
	}

	Cell& GetCell(size_t index) {
		return m_cells[index];
	}

	// Setting & moving cells

	void SetCell(
		int x, int y,
		const Cell& cell)
	{
		size_t index = GetIndex(x, y);
		SetCell(index, cell);
		KeepAlive(index);
	}

	void SetCell(
		size_t index,
		const Cell& cell)
	{
		Cell& dest = m_cells[index];

		if (    dest.Type == CellType::EMPTY
			&& cell.Type != CellType::EMPTY) // Filling a cell
		{
			std::unique_lock lock(m_filledCellCountMutex);
			m_filledCellCount++;
		}

		else if (dest.Type != CellType::EMPTY
			&&  cell.Type == CellType::EMPTY) // Removing a filled cell
		{
			std::unique_lock lock(m_filledCellCountMutex);
			m_filledCellCount--;
		}

		dest = cell;
	}

	void MoveCell(
		SandChunk* source,
		int x,   int y,
		int xto, int yto)
	{
		std::unique_lock lock(m_changesMutex);

		m_changes.emplace_back(
			source,
			source->GetIndex(x, y),
			GetIndex(xto, yto)
		);
	}

#define _DEST 2

	void CommitCells() {
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

				auto [chunk, src, dst] = m_changes[rand];

				       SetCell(dst, chunk->GetCell(src));
				chunk->SetCell(src, Cell());

				       KeepAlive(dst);
				chunk->KeepAlive(src);

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

	bool IsEmpty(int x, int y) {
		return GetCell(x, y).Type == CellType::EMPTY;
	}

	void KeepAlive(int x, int y) {
		KeepAlive(GetIndex(x, y));
	}

	void KeepAlive(size_t index) {
		std::unique_lock lock(m_workingRectMutex);

		int x = index % m_width;
		int y = index / m_width;

		m_minXw = iw::clamp<int>(min(x - 2, m_minXw), 0, m_width);
		m_minYw = iw::clamp<int>(min(y - 2, m_minYw), 0, m_height);
		m_maxXw = iw::clamp<int>(max(x + 2, m_maxXw), 0, m_width);
		m_maxYw = iw::clamp<int>(max(y + 2, m_maxYw), 0, m_height);
	}
//private:
	void UpdateRect() {
		// Update current; reset working
		m_minX = m_minXw;  m_minXw = m_width;
		m_minY = m_minYw;  m_minYw = m_height;
		m_maxX = m_maxXw;  m_maxXw = -1;
		m_maxY = m_maxYw;  m_maxYw = -1;	
	}
};

class SandWorld {
public:
	const size_t m_chunkWidth;
	const size_t m_chunkHeight;
	const double m_scale;
	std::array<std::vector<SandChunk*>, 4> m_chunkBatches;
private:
	Concurrency::concurrent_unordered_map<std::pair<int, int>, SandChunk*, iw::pair_hash> m_chunkLookup;
	std::mutex m_chunkMutex;

public:
	SandWorld(
		size_t chunkWidth,
		size_t chunkHeight,
		double scale)
		: m_chunkWidth (chunkWidth  / scale)
		, m_chunkHeight(chunkHeight / scale)
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

	void KeepAlive(int x, int y) {
		if (SandChunk* chunk = GetChunk(x, y)) {
			chunk->KeepAlive(x, y);
		}
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

	void RemoveEmptyChunks() {
		for (auto& batch : m_chunkBatches)
		for (size_t i = 0; i < batch.size(); i++) {
			SandChunk* chunk = batch.at(i);

			if (chunk->m_filledCellCount == 0) {
				m_chunkLookup.unsafe_erase(GetChunkLocation(chunk->m_x, chunk->m_y));
				batch[i] = batch.back(); batch.pop_back();
				i--;

				delete chunk;
			}
		}
	}
private:
	SandChunk* CreateChunk(
		std::pair<int, int> location)
	{
		auto [lx, ly] = location;

		if (    lx < -5 || ly < -5
		     || lx >  5 || ly >  5) // could pass in a world limit to constructor
		{
			return nullptr;
		}

		SandChunk* chunk = new SandChunk(
			m_chunkWidth, m_chunkHeight, lx, ly);

		const int c = 2;

		int batch = (c + lx % c) % c
			     + (c + ly % c) % c * c;

		m_chunkLookup.insert({ location, chunk });

		{
			std::unique_lock lock(m_chunkMutex);
			m_chunkBatches.at(batch).push_back(chunk);
		}

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

			int px = x + m_chunk->m_x;
			int py = y + m_chunk->m_y;

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
		int pingX = 0, pingY = 0;

		if (x == m_chunk->m_x)                         pingX = -1;
		if (x == m_chunk->m_x + m_chunk->m_width  - 1) pingX =  1;
		if (y == m_chunk->m_y)                         pingY = -1;
		if (y == m_chunk->m_y + m_chunk->m_height - 1) pingY =  1;

		if (pingX != 0)               m_world.KeepAlive(x + pingX, y);
		if (pingY != 0)               m_world.KeepAlive(x,         y + pingY);
		if (pingX != 0 && pingY != 0) m_world.KeepAlive(x + pingX, y + pingY);

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
