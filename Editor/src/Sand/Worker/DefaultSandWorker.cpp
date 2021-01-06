#include "Sand/Workers/DefaultSandWorker.h"
#include "iw/math/vector2.h"
#include "iw/engine/Time.h"

void DefaultSandWorker::UpdateCell(
	WorldCoord x,
	WorldCoord y,
	Cell& cell)
{
	//if (CellUpdated(x, y)) continue;

	//Cell cell = m_chunk.GetCell(x, y);

	if ((int)cell.Props == 0) {
		return;
	}

	// Would go in grav worker or something

	if (cell.Gravitised) {
		iw::vector2 dir = iw::vector2(80, 80) - iw::vector2(x, y);

		float div = 2;
		while (abs((dir / div).major()) > 1) {
			dir /= div;
			div = iw::clamp(div += iw::randf(), 1.2f, 1.8f); 
		}

		cell.dX = iw::clamp((dir.x + iw::DeltaTime()) * 0.9f, -64.f, 64.f);
		cell.dY = iw::clamp((dir.y + iw::DeltaTime()) * 0.9f, -64.f, 64.f);
	}

	const Cell& replacement = Cell::GetDefault(cell.Type);

	bool hit = false;
	int hitx, hity;

	if		(cell.Props & CellProperties::MOVE_DOWN      && MoveDown    (x, y, cell, replacement)) {}
	else if (cell.Props & CellProperties::MOVE_DOWN_SIDE && MoveDownSide(x, y, cell, replacement)) {}
	else if (cell.Props & CellProperties::MOVE_SIDE      && MoveSide    (x, y, cell, replacement)) {}
	else if (cell.Props & CellProperties::MOVE_FORWARD   && MoveForward (x, y, cell, replacement, hit, hitx, hity)) {}

	else if (cell.Props & CellProperties::MOVE_SHARED_USER && MoveAsSharedUser(x, y, cell, replacement)) {}

	if (!hit) {
		return;
	}

	if		(cell.Props & CellProperties::HIT_LIKE_BEAM)	   { HitLikeBeam(hitx, hity, x, y, cell); }
	else if (cell.Props & CellProperties::HIT_LIKE_PROJECTILE) { HitLikeProj(hitx, hity, x, y, cell); }
}

bool DefaultSandWorker::MoveDown(
	int x, int y,
	Cell& cell,
	const Cell& replace)
{
	int downX = x + cell.dX;
	int downY = y + cell.dY;

	bool down = IsEmpty(downX, downY);

	if (down) {
		MoveCell(x, y, downX, downY);
	}

	return down;
}

bool DefaultSandWorker::MoveDownSide(
	int x, int y,
	Cell& cell,
	const Cell& replace)
{
	int downLeftX = x + cell.dY + cell.dX;
	int downLeftY = y - cell.dX + cell.dY;

	int downRightX = x - cell.dY + cell.dX;
	int downRightY = y + cell.dX + cell.dY;

	bool downLeft  = IsEmpty(downLeftX,  downLeftY);
	bool downRight = IsEmpty(downRightX, downRightY);

	if (downLeft && downRight) {
		downLeft  = iw::randf() > 0;
		downRight = !downLeft;
	}

	if		(downLeft)	MoveCell(x, y, downLeftX,  downLeftY);
	else if (downRight) MoveCell(x, y, downRightX, downRightY);

	return downLeft || downRight;
}

bool DefaultSandWorker::MoveSide(
	int x, int y,
	Cell& cell,
	const Cell& replace)
{
	int leftX = x + cell.dY + cell.dX;
	int leftY = y - cell.dX;

	int rightX = x - cell.dY + cell.dX;
	int rightY = y + cell.dX;

	bool left  = IsEmpty(leftX,  leftY);
	bool right = IsEmpty(rightX, rightY);

	if (left && right) {
		left  = iw::randf() > 0;
		right = !left;
	}

	if		(left)	MoveCell(x, y, leftX,	leftY);
	else if (right) MoveCell(x, y, rightX, rightY);

	return left || right;
}

bool DefaultSandWorker::MoveForward(
	int x, int y,
	Cell& cell,
	const Cell& replace,
	bool& hit, int& hitx, int& hity)
{
	cell.Life -= iw::DeltaTime();

	if (cell.Life <= 0) {
		SetCell(x, y, Cell::GetDefault(CellType::EMPTY));
	}

	else if (cell.Speed() > 0) {
		float dsX = cell.dX * iw::DeltaTime();
		float dsY = cell.dY * iw::DeltaTime();

		float destXactual = cell.pX + dsX;
		float destYactual = cell.pY + dsY;

		std::vector<std::pair<int, int>> cellpos = FillLine(cell.pX, cell.pY, destXactual, destYactual);
		
		if (cellpos.size() == 1) { // line is only source cell because it didnt move far enough, could calc before calling FillLine ezpz
			cell.pX += dsX;
			cell.pY += dsY;
		}

		else {
			float step = 1.f / (cellpos.size() - 2);

			for (int i = 1; i < cellpos.size() - 1; i++) { // i = 0 is x, y so we dont need to check it
				int destX = cellpos[i].first;
				int destY = cellpos[i].second;

				bool forward = IsEmpty(destX, destY);

				if (forward) {
					Cell next = replace;
					next.TileId = cell.TileId;
					SetCell(cell.pX + dsX * step * i, cell.pY + dsY * step * i, next);
				}

				else if (InBounds(destX, destY)
					  && GetCell (destX, destY).TileId != cell.TileId)
				{
					hit = true;
					hitx = destX;
					hity = destY;
					break;
				}
			}

			if (!hit) {
				cell.pX = destXactual;
				cell.pY = destYactual;
				cell.Life = replace.Life;
				MoveCell(x, y, destXactual, destYactual);
			}
		}
	}

	return false;
}

bool DefaultSandWorker::MoveAsSharedUser(
	int x, int y,
	Cell& cell,
	const Cell& replace)
{
	if (!cell.User) return false;

	float s = sin(cell.User->angle);
	float c = cos(cell.User->angle);

	int px = ceil(cell.User->px + cell.pX * c - cell.pY * s);
	int py = ceil(cell.User->py + cell.pX * s + cell.pY * c);

	if (   IsEmpty(px, py)
		|| GetCell(px, py).User == cell.User)
	{
		SetCellQueued(px, py, cell);
		SetCell(x, y, Cell::GetDefault(CellType::EMPTY));
	}

	return true;
}

void DefaultSandWorker::HitLikeProj(
	int x,  int y,
	int lx, int ly,
	const Cell& bullet)
{
	float speed = bullet.Speed();

	Cell hit = GetCell(x, y);
	hit.Life -= speed;

	if (hit.Life < 0) {
		Cell cell = bullet;
		cell.dX = iw::clamp<int>(cell.dX + iw::randi(100) - 50, cell.dX * .8 - 50, cell.dX * 1.2 + 50);
		cell.dY = iw::clamp<int>(cell.dY + iw::randi(100) - 50, cell.dY * .8 - 50, cell.dY * 1.2 + 50);
		cell.Life *= 1 - iw::DeltaTime() * 5;

		SetCell(x, y, cell);
	}

	else {
		SetCell(x, y, hit);
	}
}

void DefaultSandWorker::HitLikeBeam(
	int x,  int y,
	int lx, int ly,
	const Cell& laser)
{
	float speed = laser.Speed();

	Cell hit = GetCell(x, y);
	hit.Life -= speed;

	if (   hit.Type == CellType::LASER 
		|| hit.Type == CellType::eLASER)
	{
		SetCell(lx + laser.dX, ly + laser.dY, laser);
		SetCell(lx, ly, Cell::GetDefault(CellType::EMPTY));

		return;
	}

	if (hit.Life <= 0) {
		Cell cell = laser;
		cell.dX = iw::clamp<int>(cell.dX + iw::randi(100) - 50, cell.dX * .8 - 50, cell.dX * 1.2 + 50);
		cell.dY = iw::clamp<int>(cell.dY + iw::randi(100) - 50, cell.dY * .8 - 50, cell.dY * 1.2 + 50);
		cell.Life *= 1 - iw::DeltaTime() * 5;

		SetCell(x, y, cell);
	}

	else {
		SetCell(x, y, hit);
	}
}
