#include "iw/Sand/Workers/SimpleSandWorker.h"

IW_PLUGIN_SAND_BEGIN

SimpleSandWorker::SimpleSandWorker(
	SandWorld& world,
	SandChunk* chunk)
	: SandWorker(world, chunk)
{}

void SimpleSandWorker::UpdateCell(int x, int y, Cell& cell) {
	     if (cell.Props & CellProperties::MOVE_DOWN      && MoveDown    (x, y, cell)) {}
	else if (cell.Props & CellProperties::MOVE_DOWN_SIDE && MoveDownSide(x, y, cell)) {}
	else if (cell.Props & CellProperties::MOVE_SIDE      && MoveSide    (x, y, cell)) {}
}

bool SimpleSandWorker::MoveDown(
	size_t x, size_t y,
	const Cell& cell)
{
	for (int i = 1; i > 0; i--) {
		bool down = IsEmpty(x, y - i);
		if (down) {
			MoveCell(x, y, x, y - i);
			return true;
		}
	}

	return false;
}
	
bool SimpleSandWorker::MoveDownSide(
	size_t x, size_t y,
	const Cell& cell)
{
	bool downLeft  = IsEmpty(x - 1, y - 1);
	bool downRight = IsEmpty(x + 1, y - 1);

	ShuffleIfTrue(downLeft, downRight);

	if		(downLeft)	MoveCell(x, y, x - 1, y - 1);
	else if (downRight) MoveCell(x, y, x + 1, y - 1);

	return downLeft || downRight;
}

bool SimpleSandWorker::MoveSide(
	size_t x, size_t y,
	const Cell& cell)
{
	for (int i = 1; i > 0; i--) {
		bool left  = IsEmpty(x - i, y);
		bool right = IsEmpty(x + i, y);

		ShuffleIfTrue(left, right);

			 if (left)  MoveCell(x, y, x - i, y);
		else if (right)	MoveCell(x, y, x + i, y);

		if (left || right) return true;
	}

	return false;
}

IW_PLUGIN_SAND_END
