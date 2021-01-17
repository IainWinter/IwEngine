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

	cell.Timer += __stepTime;

	if (   cell.Timer > .1
		&& (cell.Type == CellType::mLASER || cell.Type == CellType::eLASER || cell.Type == CellType::LASER)
		&& cell.Props & CellProperties::MOVE_FORWARD)
{
		if (cell.Speed() == 0 || cell.Type == CellType::mLASER) {
			(int&)cell.Props |=  int(CellProperties::MOVE_RANDOM  | CellProperties::HIT_AND_REPLACE);
			(int&)cell.Props &= ~int(CellProperties::MOVE_FORWARD | CellProperties::HIT_LIKE_BEAM);
		
			if (cell.Speed() > 0) { // only mining
				cell.Life = 1;
				cell.Precedence = 1001; // more than rez
				cell.dX = 5 * iw::randf();
				cell.dY = 5 * iw::randf();
			}

			else {
				cell.Life = .2 * (iw::randf() + 1.1f);
				cell.dX = 3 * iw::randf();
				cell.dY = 3 * iw::randf();
				cell.Precedence = 1;
			}
		}
	}

	if (cell.Gravitised) {
		iw::vector2 dir = iw::vector2(80, 80) - iw::vector2(x, y);

		float div = 2;
		while (abs((dir / div).major()) > 1) {
			dir /= div;
			div = iw::clamp(div += iw::randf(), 1.2f, 1.8f); 
		}

		cell.dX = iw::clamp((dir.x + __stepTime) * 0.9f, -64.f, 64.f);
		cell.dY = iw::clamp((dir.y + __stepTime) * 0.9f, -64.f, 64.f);
	}

	if (cell.Type == CellType::SMOKE) {
		cell.Color = iw::lerp<iw::vector4>(cell.Color, iw::Color::From255(100, 100, 100), __stepTime*10);
	}

	const Cell& replacement = Cell::GetDefault(cell.Type);

	bool hit = false;
	int hitx, hity;

		 if (cell.Props & CellProperties::MOVE_SHARED_USER && MoveAsSharedUser(x, y, cell, replacement)) {}

	else if (cell.Props & CellProperties::MOVE_DOWN      && MoveDown    (x, y, cell, replacement)) {}
	else if (cell.Props & CellProperties::MOVE_DOWN_SIDE && MoveDownSide(x, y, cell, replacement)) {}
	else if (cell.Props & CellProperties::MOVE_SIDE      && MoveSide    (x, y, cell, replacement)) {}
	else if (cell.Props & CellProperties::MOVE_RANDOM    && MoveRandom  (x, y, cell, replacement, hit, hitx, hity)) {}
	else if (cell.Props & CellProperties::MOVE_FORWARD   && MoveForward (x, y, cell, replacement, hit, hitx, hity)) {}

	if(hit) {
		UpdateHitCell(hitx, hity, x, y, cell);
	}
	
	if (cell.Props & CellProperties::DELETE_TIME && DeleteWithTime(x, y, cell)) {}
}

void DefaultSandWorker::UpdateHitCell(
	WorldCoord hx, WorldCoord hy,
	WorldCoord x,  WorldCoord y,
	Cell& cell)
{
	if (cell.Share) {
		cell.Share->Hit = true;
		cell.Share->hX = hx;
		cell.Share->hY = hy;

		if (cell.Share->RecordHitCells) {
			cell.Share->RecordHit(x, y, GetCell(hx, hy));
		}
	}

	if		(cell.Props & CellProperties::HIT_LIKE_BEAM)	   { HitLikeBeam   (hx, hy, x, y, cell); }
	else if (cell.Props & CellProperties::HIT_LIKE_PROJECTILE) { HitLikeProj   (hx, hy, x, y, cell); }
	else if (cell.Props & CellProperties::HIT_LIKE_MISSILE)    { HitLikeMissile(hx, hy, x, y, cell); }
	else if (cell.Props & CellProperties::HIT_AND_REPLACE)     { HitAndReplace (hx, hy, x, y, cell); }
}

bool DefaultSandWorker::MoveDown(
	int x, int y,
	Cell& cell,
	const Cell& replace)
{
	int downX = x + cell.dX;
	int downY = y + cell.dY;
	bool down = false;

	int i = 1;
	do {
		downX = x + cell.dX / i;
		downX = x + cell.dX / i;
		down = IsEmpty(downX, downY);
		i *= 2;
	} while (!down && downX != x && downY != y);

	if (down) {
		cell.pX += cell.dX;
		cell.pY += cell.dY;

		MoveCell(x, y, downX, downY);
	}

	else {
		cell.dX = replace.dX;
		cell.dY = replace.dY;
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
	const Cell& replace,
	bool& hit, int& hitx, int& hity)
{
	cell.pX += cell.dX * iw::randf();
	cell.pY += cell.dY * iw::randf();

	int dx = round(cell.pX);
	int dy = round(cell.pY);

	bool d = IsEmpty(dx, dy);

	if (d) {
		MoveCell(x, y, dx, dy);
	}

	else if (HasPrecedence(x, y, dx, dy)) {
		hit = true;
		hitx = dx;
		hity = dy;
		d = true;
	}

	return d;
}

bool DefaultSandWorker::MoveForward(
	int x, int y,
	Cell& cell,
	const Cell& replace,
	bool& hit, int& hitx, int& hity)
{
	//cell.Life -= iw::DeltaTime();

	if (cell.Speed() > 0 && cell.Life > 0) {
		if (cell.Share && cell.Share->UsedForMotion) {
			cell.dX = cell.Share->vX;
			cell.dY = cell.Share->vY;

			cell.Life = cell.Share->Life;

			if (cell.Share->Hit) {
				hit = true;
				hitx = x;
				hity = y;
				return true;
			}
		}

		float dsX = cell.dX * __stepTime; // need to find a way to scale velocity with time without making it 
		float dsY = cell.dY * __stepTime; // so when the framerate is so high that things dont move less than a single cell

		//ds  = iw::clamp<float>(dsX, -3, 3); // clamp to make speed for threading between chunks
		//dsY = iw::clamp<float>(dsY, -3, 3); // should be width/2, height/2 - -> +

		float destXactual = cell.pX + dsX;
		float destYactual = cell.pY + dsY;

		std::vector<std::pair<int, int>> cellpos = FillLine(cell.pX, cell.pY, destXactual, destYactual);
		
		float step = 1.f / (cellpos.size() - 1);

		for (int i = 1; i < cellpos.size(); i++) {
			bool last = i == cellpos.size() - 1;

			int destX = cellpos[i].first;
			int destY = cellpos[i].second;

			if(!InBounds(destX, destY)) break;

			cell.pX += dsX * step;
			cell.pY += dsY * step;

			if (  !IsEmpty(destX, destY)
				&& GetCell(destX, destY).TileId != cell.TileId)
			{
 				hit = true;
				hitx = destX;
				hity = destY;

				break;
			}
			
			else if (HasPrecedence(x, y, destX, destY)) {
				Cell next = replace;
				next.pX = cell.pX;
				next.pY = cell.pY;
				next.dX = last ? cell.dX : 0;
				next.dY = last ? cell.dY : 0;
				next.Share = cell.Share;
				next.TileId = cell.TileId;
				next.SplitCount = cell.SplitCount;
				next.Life = cell.Life;
				next.Timer = cell.Timer;

				SetCell(next.pX, next.pY, next);
			}
		}
	}

	KeepAlive(x, y);

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
	if (!cell.Share) return false;

	switch (cell.Share->Type) {
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

	float s = sin(cell.Share->angle);
	float c = cos(cell.Share->angle);

	int px = ceil(cell.Share->pX + cell.pX * c - cell.pY * s);
	int py = ceil(cell.Share->pY + cell.pX * s + cell.pY * c);

	if (   px != x
		|| py != y
		|| IsEmpty(px, py)
		|| GetCell(px, py).Share == cell.Share)
	{
		SetCellQueued(px, py, cell);
		SetCellQueued(x, y, Cell::GetDefault(CellType::EMPTY));
	}

	return false;
}

bool DefaultSandWorker::DeleteWithTime(
	int x, int y,
	Cell& cell)
{
	cell.Life -= __stepTime;
	if (cell.Life <= 0) {
		SetCell(x, y, Cell::GetDefault(CellType::EMPTY));

		return true;
	}

	return false;
}

// HOW HITS (SHOULD) WORK

// should remove life, damage if hit pres > beam pres, if not just change direction

void DefaultSandWorker::HitLikeProj(
	int x,  int y,
	int lx, int ly,
	Cell& bullet)
{
	bullet.Life /= 2;

	float max = Cell::GetDefault(bullet.Type).Speed();

	bullet.dX += max/5 * iw::randf();
	bullet.dY += max/5 * iw::randf();

	if (   bullet.SplitCount < bullet.MaxSplitCount
		&& HasPrecedence(x, y, lx, ly))
	{
		int px = iw::randi(2) - 1;
		int py = iw::randi(2) - 1;

		if (lx != px || ly != py) {
			bullet.SplitCount++;

			Cell c = bullet;
			c.dX += max/2 * iw::randf();
			c.dY += max/2 * iw::randf();
			c.pX += px;
			c.pY += py;

			SetCell(c.pX, c.pY, c);
		}
		
		SetCell(x, y, bullet);
	}

	else {
		//iw::vector2 v(bullet.dX, bullet.dY);
		//v.normalize();
		//v /= v.minor();

		//if (IsEmpty(x+v.x, y+v.y)) {
		//	SetCell(x+v.x, y+v.y, bullet);
		//}
	}
}

void RandomizeVector(iw::vector2& v, float max, float a) {
	v = iw::clamp<iw::vector2>(v + max / a * iw::vector2(iw::randf(), iw::randf()), -max, max);
	v.normalize();
	v *= max;
}

void DefaultSandWorker::HitLikeBeam(
	int x,  int y,
	int lx, int ly,
	Cell& beam)
{
	beam.Life /= 2;

	float max = Cell::GetDefault(beam.Type).Speed();
	iw::vector2 v(beam.dX, beam.dY);

	RandomizeVector(v, max, 5);
	beam.dX = v.x;
	beam.dY = v.y;

	Cell hit = GetCell(x, y);

	if (hit.Precedence > beam.Precedence) {
		hit.Life -= beam.Speed();
	}

	if (   beam.SplitCount < beam.MaxSplitCount
		&& hit.Life <= 0)
	{
		int px = iw::randi(2) - 1;
		int py = iw::randi(2) - 1;

		if (lx != px || ly != py) {
			beam.SplitCount++;

			RandomizeVector(v, max, 10);

			Cell c = beam;
			c.dX = v.x;
			c.dY = v.y;
			c.pX += px;
			c.pY += py;

			SetCell(c.pX, c.pY, c);
		}

		SetCell(x, y, beam);
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
	
	int size = 60 + iw::randi(20);

	for(int i = -size; i < size; i++)
	for(int j = -size; j < size; j++) {
		int dx = x + i;
		int dy = y + j;

		const Cell& dest = GetCell(dx, dy);

		if (   /*dest.TileId != missile.TileId
			&& */iw::randf() > 0
			&& sqrt(i*i+j*j) < (35.f + iw::randf() * 15.f))
		{
			bool smoke = iw::randf() > 0;

			Cell cell = smoke
				? Cell::GetDefault(CellType::SMOKE)
				: Cell::GetDefault(CellType::EXPLOSION);

			cell.pX = x + i;
			cell.pY = y + j;
			cell.dX *= smoke ? 5 : 1;
			cell.dY *= smoke ? 5 : 1;
			cell.TileId = missile.TileId;
			cell.Life *= smoke ? 10+iw::randf()*3 : 1 / cell.Speed() * (iw::randf() + 1) * 2;
			cell.Timer = missile.Timer;

			SetCell(x+i, y+j, cell);
		}

		else if (dest.Type == CellType::MISSILE) {
			dest.Share->Hit = true;
			dest.Share->hX = x;
			dest.Share->hY = y;
		}
	}
}

void DefaultSandWorker::HitAndReplace(
	int x, int y,
	int cx, int cy,
	Cell& cell)
{
	SetCell(x, y, cell);
	SetCell(cx, cy, Cell::GetDefault(CellType::EMPTY)); // could eject hit cell with velocity
}
