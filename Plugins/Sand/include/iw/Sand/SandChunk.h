#pragma once

#include "Cell.h"
#include <vector>
#include <mutex>
#include <algorithm>

IW_PLUGIN_SAND_BEGIN

class SandChunk {
public:
	Cell* m_cells;

	const int m_width, m_height;
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
	IW_PLUGIN_SAND_API SandChunk(int width, int height, int x, int y);
	IW_PLUGIN_SAND_API ~SandChunk();

	// Getting cells

	IW_PLUGIN_SAND_API Cell& GetCell(int x, int y);
	IW_PLUGIN_SAND_API Cell& GetCell(size_t index);

	// Setting & moving cells

	IW_PLUGIN_SAND_API void SetCell(int x, int y, const Cell& cell);
	IW_PLUGIN_SAND_API void SetCell(size_t index, const Cell& cell);

	IW_PLUGIN_SAND_API void MoveCell(SandChunk* source, int x, int y, int xto, int yto);
	IW_PLUGIN_SAND_API void CommitCells();

	// Helpers

	IW_PLUGIN_SAND_API size_t GetIndex(int x, int y);

	IW_PLUGIN_SAND_API bool InBounds(int x, int y);
	IW_PLUGIN_SAND_API bool IsEmpty (int x, int y);

	IW_PLUGIN_SAND_API void KeepAlive(int x, int y);
	IW_PLUGIN_SAND_API void KeepAlive(size_t index);

	IW_PLUGIN_SAND_API void UpdateRect();
};

IW_PLUGIN_SAND_END
