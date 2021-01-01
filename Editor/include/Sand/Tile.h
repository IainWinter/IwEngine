#pragma once

#include "Cell.h"
#include <vector>

struct Tile {
	std::vector<WorldCoords> Positions;
	std::vector<Cell> Cells;

	const int InitialCellCount = 0;
	int TileId = 0;

	Tile() = default;

	Tile(
		std::vector<WorldCoords>&& positions,
		std::vector<Cell>&& cells)
		: Positions(positions)
		, Cells(cells)
		, InitialCellCount(CellCount())
	{
		assert(positions.size() == cells.size()); // dont assert, fill cells or remove cells based on positions

		static int tileId = 0;
		TileId = ++tileId;

		for (Cell& c : Cells) {
			c.TileId = TileId;
		}
	}

	void RemoveCell(size_t i) {
		Positions[i] = Positions.back(); Positions.pop_back();
		Cells    [i] = Cells    .back(); Cells    .pop_back();
	}

	size_t CellCount() {
		return Positions.size();
	}
};
