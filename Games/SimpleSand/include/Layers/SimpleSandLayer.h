#pragma once 

#include "iw/engine/Layer.h"

#include "Sand/SimpleSandWorld.h"
//#include "Sand/Tile.h"

struct Tile {
	std::vector<std::pair<int, int>> Positions;
	float X = 0;
	float Y = 0;
};

class SimpleSandWorker : public SandWorker {
public:
	SimpleSandWorker(SandWorld& world, SandChunk* chunk) : SandWorker(world, chunk) {}

	void UpdateCell(int x, int y, Cell& cell) override {
			if (cell.Props & CellProperties::MOVE_DOWN      && MoveDown    (x, y, cell)) {}
		else if (cell.Props & CellProperties::MOVE_DOWN_SIDE && MoveDownSide(x, y, cell)) {}
		else if (cell.Props & CellProperties::MOVE_SIDE      && MoveSide    (x, y, cell)) {}
	}
private:
	bool MoveDown    (size_t x, size_t y, const Cell& cell);
	bool MoveDownSide(size_t x, size_t y, const Cell& cell);
	bool MoveSide    (size_t x, size_t y, const Cell& cell);
};

namespace iw {
	class SimpleSandLayer
		: public Layer
	{
	private:
		iw::Mesh m_sandMesh;
		iw::ref<iw::Texture> m_sandTexture;

		SandWorld m_world;
		std::vector<Tile> m_tiles;

		Cell _EMPTY;
		Cell _SAND;
		Cell _WATER;
		Cell _ROCK;

	public:
		SimpleSandLayer();

		int Initialize() override;
		void PostUpdate() override;
	};
}

inline void ShuffleIfTrue(bool& a, bool& b) {
	if (a && b) {
		a = iw::randf() > 0;
		b = !a;
	}
}


