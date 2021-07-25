#include "iw/Sand/Workers/SimpleSandWorker.h"

IW_PLUGIN_SAND_BEGIN

SimpleSandWorker::SimpleSandWorker(
	SandWorld& world,
	SandChunk* chunk)
	: SandWorker(world, chunk)
{}

void SimpleSandWorker::UpdateCell(int x, int y, Cell& cell) {
	     if (cell.Props & CellProp::MOVE_FORCE     && MoveForce   (x, y, cell)) {}
	else if (cell.Props & CellProp::MOVE_DOWN      && MoveDown    (x, y, cell)) {}
	else if (cell.Props & CellProp::MOVE_DOWN_SIDE && MoveDownSide(x, y, cell)) {}
	else if (cell.Props & CellProp::MOVE_SIDE      && MoveSide    (x, y, cell)) {}
	else if (cell.Props & CellProp::MOVE_RANDOM    && MoveRandom  (x, y, cell)) {}
}

bool SimpleSandWorker::MoveForce(
	int x, int y,
	Cell& cell)
{
	float xt = cell.x + cell.dx;
	float yt = cell.y + cell.dy;

	bool differnt = int(xt) != x || int(yt) != y;

	if (differnt) {
		if (!IsEmpty(xt, yt)) {
			cell.dx *= .5f;
			cell.dy *= .5f;

			return false;
		}

		MoveCell(x, y, xt, yt);
	}

	cell.x = xt;
	cell.y = yt;

	return differnt;

	//float dx = cell.dx;
	//float dy = cell.dy;

	//int count = sqrt(dx * dx + dy * dy);

	//if (count == 0) return false;

	//float step = 1.f / count;

	//int i; float X, Y;
	//for (i = 1; i <= count; i++) {
	//	X = cell.x + dx * step * i;
	//	Y = cell.y + dy * step * i;
	//	if (!IsEmpty(X, Y)) {
	//		X -= dx * step; // Is there a better way to do this?
	//		Y -= dy * step;
	//		break;
	//	}
	//}

	//bool moved = i != 1;

	//if (moved) {
	//	MoveCell(x, y, X, Y);
	//}

	//else {
	//	cell.dx = 0;
	//	cell.dy = 0;
	//}

	//return moved;
}

bool SimpleSandWorker::MoveDown(
	int x, int y,
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
	int x, int y,
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
	int x, int y,
	const Cell& cell)
{
	bool left  = IsEmpty(x - 1, y);
	bool right = IsEmpty(x + 1, y);

	ShuffleIfTrue(left, right);

		 if (left)  MoveCell(x, y, x - 1, y);
	else if (right)	MoveCell(x, y, x + 1, y);

	return left || right;
}

bool SimpleSandWorker::MoveRandom(
	int x, int y,
	const Cell& cell)
{

	//cell.x += cell.dx * iw::randf();
	//cell.y += cell.dy * iw::randf();


	int xx = x + (iw::randi(2) - 1) * (cell.dx + 1);
	int yy = y + (iw::randi(2) - 1) * (cell.dy + 1);

	bool r = IsEmpty(xx, yy);

	if (r) MoveCell (x, y, xx, yy);
	else KeepAlive(x, y);

	return r;
}

IW_PLUGIN_SAND_END
