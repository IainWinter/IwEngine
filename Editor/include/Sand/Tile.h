#pragma once

#include "Cell.h"
#include <vector>

struct Tile {
	std::vector<iw::vector2> Locations;
	const int InitialLocationsSize = 0;
	int TileId = 0;

	Tile() = default;

	Tile(std::vector<iw::vector2> locations, int scale)
		: InitialLocationsSize(locations.size() * scale)
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

//struct Tile {
//	std::vector<WorldCoords> Positions;
//	std::vector<Cell> Cells;
//
//	const int InitialCellCount = 0;
//	int TileId = 0;
//
//	Tile() = default;
//
//	Tile(
//		std::vector<WorldCoords>&& positions,
//		std::vector<Cell>&& cells)
//		: Positions(positions)
//		, Cells(cells)
//		, InitialCellCount(CellCount())
//	{
//		assert(positions.size() == cells.size()); // dont assert, fill cells or remove cells based on positions
//
//		static int tileId = 0;
//		TileId = ++tileId;
//
//		for (Cell& c : Cells) {
//			c.TileId = TileId;
//		}
//	}
//
//	void RemoveCell(size_t i) {
//		Positions[i] = Positions.back(); Positions.pop_back();
//		Cells    [i] = Cells    .back(); Cells    .pop_back();
//	}
//
//	size_t CellCount() {
//		return Positions.size();
//	}
//};
