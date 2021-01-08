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

		 if (cell.Props & CellProperties::MOVE_SHARED_USER && MoveAsSharedUser(x, y, cell, replacement)) {}

	else if (cell.Props & CellProperties::MOVE_DOWN      && MoveDown    (x, y, cell, replacement)) {}
	else if (cell.Props & CellProperties::MOVE_DOWN_SIDE && MoveDownSide(x, y, cell, replacement)) {}
	else if (cell.Props & CellProperties::MOVE_SIDE      && MoveSide    (x, y, cell, replacement)) {}
	else if (cell.Props & CellProperties::MOVE_RANDOM    && MoveRandom  (x, y, cell, replacement)) {}
	else if (cell.Props & CellProperties::MOVE_FORWARD   && MoveForward (x, y, cell, replacement, hit, hitx, hity)) {}

	if      (hit)
	if		(cell.Props & CellProperties::HIT_LIKE_BEAM)	   { HitLikeBeam   (hitx, hity, x, y, cell); }
	else if (cell.Props & CellProperties::HIT_LIKE_PROJECTILE) { HitLikeProj   (hitx, hity, x, y, cell); }
	else if (cell.Props & CellProperties::HIT_LIKE_MISSILE)    { HitLikeMissile(hitx, hity, x, y, cell); }
	
	if (cell.Props & CellProperties::DELETE_TIME && DeleteWithTime(x, y, cell)) {}
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
		cell.pX += cell.dX;
		cell.pY += cell.dY;

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

bool DefaultSandWorker::MoveRandom(
	int x, int y,
	Cell& cell,
	const Cell& replace)
{
	cell.pX += cell.dX * iw::randf();
	cell.pY += cell.dY * iw::randf();

	int dx = round(cell.pX);
	int dy = round(cell.pY);

	bool d = IsEmpty(dx, dy) || GetCell(x, y).Precedence < cell.Precedence;

	if (d) {
		MoveCell(x, y, dx, dy);
	}

	return d;
}

bool DefaultSandWorker::MoveForward(
	int x, int y,
	Cell& cell,
	const Cell& replace,
	bool& hit, int& hitx, int& hity)
{
	if (cell.Speed() > 0) {
		if (cell.User) {
			cell.dX = cell.User->vX;
			cell.dY = cell.User->vY;
		}

		iw::vector2 minv(cell.dX, cell.dY);
		minv.normalize();
		minv *= 1 / iw::DeltaTime();
		minv *= cell.Speed();

		float dsX = minv.x * iw::DeltaTime();
		float dsY = minv.y * iw::DeltaTime();

		//ds  = iw::clamp<float>(dsX, -3, 3);
		//dsY = iw::clamp<float>(dsY, -3, 3);

		float destXactual = cell.pX + dsX;
		float destYactual = cell.pY + dsY;

		std::vector<std::pair<int, int>> cellpos = FillLine(cell.pX, cell.pY, destXactual, destYactual);
		
		float step = 1.f / (cellpos.size() - 1);

		for (int i = 1; i < cellpos.size(); i++) {
			bool last = i == cellpos.size() - 1;

			int destX = cellpos[i].first;
			int destY = cellpos[i].second;

			bool forward = IsEmpty(destX, destY);

			if (forward) {
				Cell next = replace;
				next.pX = cell.pX + dsX * step * i;
				next.pY = cell.pY + dsY * step * i;
				next.dX = last ? cell.dX : 0;
				next.dY = last ? cell.dY : 0;
				next.User = cell.User;
				next.TileId = cell.TileId;
				next.SplitCount = cell.SplitCount;
				SetCell(next.pX, next.pY, next);
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
	}

	if (!hit) {
		cell.dX = 0;
		cell.dY = 0;
	}

	return false;
}

bool DefaultSandWorker::MoveAsSharedUser(
	int x, int y,
	Cell& cell,
	const Cell& replace)
{
	if (!cell.User) return false;

	switch (cell.User->Type) {
		case SharedCellType::ASTEROID: {
			//if (CurrentTick() % 2 == 0) {
			//	float dx = cell.dX;
			//	float dy = cell.dY;
			//
			//	iw::vector2 dir(cell.User->pX - cell.pX, cell.User->pY - cell.pY);
			//
			//	float div = 2;
			//	while (abs((dir / div).major()) > 1) {
			//		dir /= div;
			//		div = iw::clamp(div += iw::randf(), 1.2f, 1.8f);
			//	}
			//
			//	cell.dX = dir.x;
			//	cell.dY = dir.y;

			//	/*cell.dX = iw::clamp<int>(cell.User->cX - cell.pX, -1, 1);
			//	cell.dY = iw::clamp<int>(cell.User->cY - cell.pY, -1, 1);*/
			//
			//	MoveDown    (x, y, cell, replace);
			//	MoveSide    (x, y, cell, replace);
			//	MoveDownSide(x, y, cell, replace);
			//
			//	cell.dX = dx;
			//	cell.dY = dy;
			//
			//  return false;
			//}
			break;
		}
	}

	float s = sin(cell.User->angle);
	float c = cos(cell.User->angle);

	int px = ceil(cell.User->pX + cell.pX * c - cell.pY * s);
	int py = ceil(cell.User->pY + cell.pX * s + cell.pY * c);

	if (   px != x
		|| py != y
		|| IsEmpty(px, py)
		|| GetCell(px, py).User == cell.User)
	{
		SetCellQueued(px, py, cell);
		SetCell(x, y, Cell::GetDefault(CellType::EMPTY));
	}

	return false;
}

bool DefaultSandWorker::DeleteWithTime(
	int x, int y,
	Cell& cell)
{
	cell.Life -= iw::DeltaTime();
	if (cell.Life < 0) {
		SetCell(x, y, Cell::GetDefault(CellType::EMPTY));

		return true;
	}

	return false;
}

void DefaultSandWorker::HitLikeProj(
	int x,  int y,
	int lx, int ly,
	Cell& bullet)
{
	float speed = bullet.Speed();

	Cell hit = GetCell(x, y);
	hit.Life -= speed;

	bullet.dX = iw::clamp(bullet.dX + iw::randf()*2, -10.f, 10.f);
	bullet.dY = iw::clamp(bullet.dY + iw::randf()*2, -10.f, 10.f);

	if (hit.Life < 0 && bullet.SplitCount < 2) {
		bullet.SplitCount++;
		SetCell(x, y, bullet);
		SetCell(x+iw::randi(2)-1, y+iw::randi(2)-1, bullet);
	}

	else {
		SetCell(x, y, hit);
	}
}

void DefaultSandWorker::HitLikeBeam(
	int x,  int y,
	int lx, int ly,
	Cell& laser)
{
	float speed = laser.Speed();

	Cell hit = GetCell(x, y);
	hit.Life -= speed;

	//if (   hit.Type == CellType::LASER 
	//	|| hit.Type == CellType::eLASER)
	//{
	//	SetCell(lx + laser.dX, ly + laser.dY, laser);
	//	SetCell(lx, ly, Cell::GetDefault(CellType::EMPTY));

	//	return;
	//}

	laser.SplitCount++;
	laser.dX = iw::clamp(laser.dX + iw::randf() * 5, -10.f, 10.f);
	laser.dY = iw::clamp(laser.dY + iw::randf() * 5, -10.f, 10.f);

	if (hit.Life <= 0 && laser.SplitCount < 10) {
		Cell cell = laser;
		//cell.Life -= iw::DeltaTime() * 2;

		SetCell(x, y, cell);
	}

	else {
		SetCell(x, y, hit);
	}
}

void DefaultSandWorker::HitLikeMissile(
	int x,  int y,
	int mx, int my,
	Cell& missile)
{
	float speed = missile.Speed();

	missile.Life = 0;
	
	for(int i = -50; i < 50; i++)
	for(int j = -50; j < 50; j++) {
		if (   iw::randf() > 0 && sqrt(i*i+j*j) < 35 + iw::randf() * 15
			&& GetCell(x+i, y+j).TileId != missile.TileId)
		{
			Cell cell = iw::randf() > 0
				? Cell::GetDefault(CellType::SMOKE)
				: Cell::GetDefault(CellType::EXPLOSION);

			cell.pX = x + i;
			cell.pY = y + j;
			cell.dX *= 5;
			cell.dY *= 5;
			cell.TileId = missile.TileId;
			cell.Life *= (iw::randf() + 1) * 2;

			SetCell(x+i, y+j, cell);
		}
	}
}
