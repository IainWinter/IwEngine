#include "Layers/SandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

#include "iw/math/matrix.h"

std::unordered_map<CellType, Cell> Cell::m_defaults = {};

namespace iw {
	SandLayer::SandLayer()
		 : Layer("Ray Marching")
		, shader(nullptr)
		, target (nullptr)
		, world(SandWorld(200, 200, 2))
	{
		//matrix<>

		//vector<2> t = m * d3;

		srand(time(nullptr));
	}

	int SandLayer::Initialize() {
		Cell empty  = { CellType::EMPTY };
		Cell sand   = { CellType::SAND,  CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE };
		Cell water  = { CellType::WATER, CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE };
		Cell rock   = { CellType::ROCK  };
		Cell metal  = { CellType::METAL };
		Cell debris = { CellType::DEBRIS };
		Cell laser  = { CellType::LASER,  CellProperties::MOVE_FORWARD | CellProperties::HIT_LIKE_BEAM };
		Cell elaser = { CellType::eLASER, CellProperties::MOVE_FORWARD | CellProperties::HIT_LIKE_BEAM };
		Cell bullet = { CellType::BULLET, CellProperties::MOVE_FORWARD | CellProperties::HIT_LIKE_PROJECTILE };

		empty  .Color = iw::Color::From255(  0,   0,   0);
		sand   .Color = iw::Color::From255(237, 201, 175);
		water  .Color = iw::Color::From255(175, 201, 237);
		rock   .Color = iw::Color::From255(201, 201, 201);
		metal  .Color = iw::Color::From255(230, 230, 230);
		debris .Color = iw::Color::From255(150, 150, 150);
		laser  .Color = iw::Color::From255(255,   0,   0);
		elaser .Color = iw::Color::From255(  0, 200, 255);
		bullet .Color = iw::Color::From255(255, 255,   0);

		laser .Life = 0.06f;
		elaser.Life = 1.00f;
		bullet.Life = 0.02f;

		Cell::SetDefault(CellType::EMPTY,  empty);
		Cell::SetDefault(CellType::SAND,   sand);
		Cell::SetDefault(CellType::WATER,  water);
		Cell::SetDefault(CellType::ROCK,   rock);
		Cell::SetDefault(CellType::METAL,  metal);
		Cell::SetDefault(CellType::DEBRIS, debris);
		Cell::SetDefault(CellType::LASER,  laser);
		Cell::SetDefault(CellType::eLASER, elaser);
		Cell::SetDefault(CellType::BULLET, bullet);

		//for (float x = 0; x < 2; x += .1) 
		//for (float y = 0; y < 2; y += .1) {
		//	vector<2> in(x, y);

		//	matrix<2, 2> layer1;
		//	layer1.columns[0] = { -1,  1 };
		//	layer1.columns[1] = {  1, -1 };

		//	vector<2> after1 = layer1 * in - vector<2>(.6, 1.4);

		//	for (float& f : after1.components) {
		//		if (f > 0) f = 1;
		//		else	   f = 0;
		//	}

		//	matrix<1, 2> layer2;
		//	layer2.columns[0] = { 1, 1 };

		//	vector<1> out = layer2 * after1 - vector<1>(-.8);

		//	for (float& f : out.components) {
		//		if (f > 0) f = 1;
		//		else	   f = 0;
		//	}

		//	LOG_INFO << "(" << x << "," << y << ") = " << out;
		//}

		if (int err = Layer::Initialize()) { 
			return err;
		}

		shader = Asset->Load<Shader>("shaders/texture.shader");

		texture = REF<iw::Texture>(
			Renderer->Width()  / world.m_scale,
			Renderer->Height() / world.m_scale, 
			iw::TEX_2D,
			iw::RGBA
		);
		texture->SetFilter(iw::NEAREST);
		texture->CreateColors();
		texture->Clear();

		target = REF<RenderTarget>();
		target->AddTexture(texture);

		reset = true;

		return 0;
	}

	void SandLayer::PostUpdate() {
		// Game update

		world.m_currentTick += 1;

		if (reset) {
			world.Reset();

			//for (int x = -300; x < 300; x++)
			//for (int y = -200; y < -50; y++) {
			//	world.SetCell(x, y, _SAND);
			//}

			//for (int x = -(int)texture->Width()/2; x < (int)texture->Width()/2; x++)
			//for (int y = -100; y < -50; y++) {
			//	world.SetCell(x, y, _WATER);
			//}

			Space->Clear();

			player = Space->CreateEntity<iw::Transform, Tile, Player>();

			player.Set<iw::Transform>();
			player.Set<Tile>(std::vector<iw::vector2> {
								   vector2(1, 0),
					vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
					vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
					vector2(0, 3),							     vector2(3, 3),
					vector2(0, 4),							     vector2(3, 4)
			}, 5);
			player.Set<Player>();

			reset = false;
		}

		spawnEnemy -= iw::DeltaTime();
		if (spawnEnemy < 0) {
			iw::Entity enemy = Space->CreateEntity<iw::Transform, Tile, Enemy2>();
			enemy.Set<iw::Transform>(iw::vector2(texture->Width() * iw::randf(), texture->Height() + 100 * (1+iw::randf())));
			enemy.Set<Tile>(std::vector<iw::vector2> {
								   vector2(1, 0),				 vector2(3, 0),
					vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
					vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
					vector2(0, 3),								 vector2(3, 3),
					vector2(0, 4),								 vector2(3, 4)
			}, 2);
			enemy.Set<Enemy2>(iw::vector2(texture->Width() * .4f * iw::randf(), (texture->Height() - 200) * .4f * (iw::randf() + 1)));

			spawnEnemy = iw::randf() + 10;
		}

		 // camera frustrum, mostly used at the bottom, but needed for screen mouse pos to world pos

		vector2 playerLocation = player.Find<iw::Transform>()->Position;

		int height = target->Tex(0)->Height();
		int width  = target->Tex(0)->Width();

		int fy = playerLocation.y - height / 2;
		int fx = playerLocation.x - width  / 2;
		int fy2 = fy + height;
		int fx2 = fx + width;

		vector2 pos = mousePos / world.m_scale;
		pos.x = floor(pos.x) + fx;
		pos.y = floor(texture->Height() - pos.y) + fy;
		
		gravPos = pos;

		if(Keyboard::KeyDown(iw::E) || Keyboard::KeyDown(iw::C) || Keyboard::KeyDown(iw::F) || Keyboard::KeyDown(iw::R)) {
			Cell cell;
			if (Keyboard::KeyDown(iw::C)) {
				cell = Cell::GetDefault(CellType::SAND);
			}

			else if (Keyboard::KeyDown(iw::F)) {
				cell = Cell::GetDefault(CellType::WATER);
			}

			else if (Keyboard::KeyDown(iw::E)) {
				cell = Cell::GetDefault(CellType::EMPTY);
			}

			else if (Keyboard::KeyDown(iw::R)) {
				cell = Cell::GetDefault(CellType::ROCK);
			}
			
			for (int  i = 0; i  < 20; i++)
			for (int ii = 0; ii < 20; ii++) {
				world.SetCell(pos.x + i, pos.y + ii, cell);
			}
		}

		Space->Query<iw::Transform, Tile>().Each([&](
			auto e,
			auto t,
			auto a)
		{
			if (a->Locations.size() < a->InitialLocationsSize / 3) {
				if ((*Space).HasComponent<Player>(e)) {
					reset = true;
				}

				(*Space).QueueEntity(e, iw::func_Destroy);
				return;
			}

			for (int i = 0; i < a->Locations.size(); i++) {
				vector2 v = vector4(a->Locations[i], 0, 1) * t->Transformation().transposed();

				if (!world.InBounds(v.x, v.y)) continue;
				
				const Cell& cell = world.GetCell(v.x, v.y);

				if (   cell.Type   == CellType::EMPTY
					|| cell.TileId == a->TileId
					||(cell.Type == CellType::eLASER && cell.Speed() == 0))
				{
					Cell me = Cell::GetDefault(CellType::METAL);
					me.TileId = a->TileId;
					me.Life = 1;

					world.SetCell(v.x, v.y, me);
				}

				else {
					a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
					i--;
				}
			}
		});

		Space->Query<iw::Transform, Tile, Player>().Each([&](
			auto,
			auto t,
			auto a,
			auto p)
		{
			p->FireTimeout -= iw::DeltaTime();

			if (   p->FireTimeout < 0
				&& p->FireButtons != 0)
			{
				p->FireTimeout = p->FireButtons.x == 1 ? .05f : .001f;

				float speed      = p->FireButtons.x == 1 ?                                600 :                              1500;
				const Cell& shot = p->FireButtons.x == 1 ? Cell::GetDefault(CellType::BULLET) : Cell::GetDefault(CellType::LASER);

				Fire(t->Position, pos, speed, shot, a->TileId);
			}
		});

		Space->Query<iw::Transform, Tile, Enemy2>().Each([&](
			auto,
			auto t,
			auto a,
			auto p)
		{
			p->FireTimeout -= iw::DeltaTime();

			if (p->FireTimeout < 0) {
				p->FireTimeout = (iw::randf() + 2) * 2;

				Fire(t->Position, playerLocation, 800, Cell::GetDefault(CellType::eLASER), a->TileId);
			}
		});

		// Sand update

		//stepTimer -= iw::DeltaTime();
		//if (stepTimer < 0 /*&& Keyboard::KeyDown(V)*/) {
		//	stepTimer = 1 / 180.0f;

			std::mutex chunkCountMutex;
			std::condition_variable chunkCountCV;
			int chunkCount = 0;

			for(auto& pair: world.m_chunks) {
				if (pair.second.IsEmpty()) {
					continue;
				}

				if ((iw::vector2(pair.second.m_x, pair.second.m_y) - playerLocation).length() > fx2 - fx) {
					continue;
				}

				{
					std::unique_lock lock(chunkCountMutex);
					chunkCount++;
				}

				Task->queue([&]() {
					SandWorker(world, pair.second).UpdateChunk();

					{
						std::unique_lock lock(chunkCountMutex);
						chunkCount--;
					}

					chunkCountCV.notify_one();
				});
			}

			//LOG_INFO << "Need to update " << chunkCount << " chunks. dt is " << iw::DeltaTime() << " seconds (" << 1 / iw::DeltaTime() << "fps)";

			std::unique_lock lock(chunkCountMutex);
			chunkCountCV.wait(lock, [&](){ return chunkCount == 0; });
		
		//}

		// Swap buffers

		target->Tex(0)->Clear();
		unsigned int* colors = (unsigned int*)target->Tex(0)->Colors();

		//m_stars.seed(1);

		for (SandChunk* chunk : world.GetVisibleChunks(fx, fy, fx2, fy2)) {
			int startY = iw::clamp(fy  - chunk->m_y, 0, chunk->m_height);
			int startX = iw::clamp(fx  - chunk->m_x, 0, chunk->m_width);
			int endY   = iw::clamp(fy2 - chunk->m_y, 0, chunk->m_height);
			int endX   = iw::clamp(fx2 - chunk->m_x, 0, chunk->m_width);

			for (int y = startY; y < endY; y++)
			for (int x = startX; x < endX; x++) {
				int texi = (chunk->m_x + x - fx) + (chunk->m_y + y - fy) * width;

#undef max

				//if (m_stars() < m_stars.max() / 2000) {
				//	colors[texi] = INT_MAX;
				//}

				//const Cell& cell = chunk->GetCellUnsafe(x, y);

				//if (cell.Type != CellType::EMPTY) {
					colors[texi] = chunk->GetCellUnsafe(x, y).Color; // assign rgba at the same time
				//}

				//else {
				//}

				/*
				if (x == 0 || y == 0) {
					colors[texi] = iw::Color(1, 0, 0, 1);
				}

				if (Keyboard::KeyDown(K)) {
					if (x == 0 || y == 0) {
						colors[texi] = iw::Color(1, 0, 0, 1);
					}
				}
				*/
			}
		}

		Space->Query<iw::Transform, Tile>().Each([&](
			auto e,
			auto t,
			auto a)
		{
			for (int i = 0; i < a->Locations.size(); i++) {
				vector2 v = vector4(a->Locations[i], 0, 1) * t->Transformation().transposed();

				if (!world.InBounds(v.x, v.y)) continue;
				
				const Cell& cell = world.GetCell(v.x, v.y);

				if (cell.Type == CellType::EMPTY) continue;

				if (cell.TileId == a->TileId) {
					world.SetCell(v.x, v.y, Cell::GetDefault(CellType::EMPTY));
				}

				else {
					a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
					i--;
				}
			}
		});
		
		Space->Query<iw::Transform, Player>().Each([&](
			auto,
			auto t,
			auto p)
		{
			p->Velocity *= 1 - iw::DeltaTime();
			p->Velocity = iw::clamp<float>(p->Velocity + p->Movement.y * iw::DeltaTime() * 3, -15, 15);

			t->Position += t->Up() * p->Velocity;
			t->Rotation *= iw::quaternion::from_euler_angles(0, 0, -p->Movement.x * iw::DeltaTime());
		});

		Space->Query<iw::Transform, Enemy2>().Each([&](
			auto e,
			auto t,
			auto p)
		{
			t->Position = iw::lerp<vector2>(
				t->Position,
				iw::vector2(
					p->Spot.x + cos(iw::TotalTime() + e.Index / 5) * 100,
					p->Spot.y + sin(iw::TotalTime() + e.Index / 5) * 100
				),
				iw::DeltaTime());
		});

		target->Tex(0)->Update(Renderer->Device); // should be auto in apply filter
		Renderer->ApplyFilter(shader, target, nullptr);
	}

	bool SandLayer::On(
		MouseMovedEvent& e)
	{
		mousePos = { e.X, e.Y };
		player.Find<Player>()->MousePos = mousePos;
		return false;
	}

	bool SandLayer::On(
		MouseButtonEvent& e)
	{
		switch (e.Button) {
			case iw::val(LMOUSE): player.Find<Player>()->FireButtons.x = e.State ? 1 : 0; break;
			case iw::val(RMOUSE): player.Find<Player>()->FireButtons.y = e.State ? 1 : 0; break;
		}

		return false;
	}

	bool SandLayer::On(
		KeyEvent& e)
	{
		Player* p = player.Find<Player>();
		switch (e.Button) {
			case iw::val(D): {
				if (e.State == 1 && p->Movement.x == 1) break;
				p->Movement.x += e.State ? 1 : -1; break;
			}
			case iw::val(A): {
				if (e.State == 1 && p->Movement.x == -1) break;
				p->Movement.x -= e.State ? 1 : -1; break;
			}
							 
			case iw::val(W): {
				if (e.State == 1 && p->Movement.y == 1) break;
				p->Movement.y += e.State ? 1 : -1; break;
			}
			case iw::val(S): {
				if (e.State == 1 && p->Movement.y == -1) break;
				p->Movement.y -= e.State ? 1 : -1; break;
			}
		}

		return false;
	}

	std::vector<iw::vector2> FillLine(
		int x,  int y,
		int x2, int y2)
	{
		std::vector<iw::vector2> positions; // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

		int dx =  abs(x2 - x);
		int dy = -abs(y2 - y);
		int sx = x < x2 ? 1 : -1;
		int sy = y < y2 ? 1 : -1;
		int err = dx + dy;  /* error value e_xy */
		
		while (true) {
			positions.emplace_back(x, y);

			if (x == x2 && y == y2) break;
			
			int e2 = 2 * err;
			if (e2 >= dy) { /* e_xy + e_x > 0 */
				err += dy;
				x += sx;
			}

			if (e2 <= dx) { /* e_xy + e_y < 0 */
				err += dx;
				y += sy;
			}
		}

		return positions;
	}
}

void SandWorker::UpdateChunk() {
	for (int x = 0;                  x <  m_chunk.m_width; x++)
	for (int y = m_chunk.m_height-1; y >= 0;               y--) {
		
		if (CellAlreadyUpdated(x, y)) continue;

		Cell cell = m_chunk.GetCell(x, y);

		// Would go in grav worker or something

		//if (cell.Gravitised) {
		//	vector2 dir = gravPos - vector2(x, y);

		//	float div = 2;
		//	while (abs((dir / div).major()) > 1) {
		//		dir /= div;
		//		div = iw::clamp(div - iw::randf(), 1.2f, 2.f);
		//	}

		//	Cell& scell = chunk.GetCell(x, y, true);

		//	cell.Direction  += dir + iw::DeltaTime();
		//	scell.Direction += dir + iw::DeltaTime();

		//	cell.Direction  *= .9f;
		//	scell.Direction *= .9f;

		//	cell.Direction  = iw::clamp<vector2>(cell.Direction, -5, 5);
		//	scell.Direction = iw::clamp<vector2>(scell.Direction, -5, 5);
		//}

		const Cell& replacement = Cell::GetDefault(cell.Type);

		bool moved = true;
		
		bool hit = false;
		int hitx, hity;

		if		(cell.Props & CellProperties::MOVE_DOWN      && MoveDown    (x, y, cell, replacement)) {}
		else if (cell.Props & CellProperties::MOVE_DOWN_SIDE && MoveDownSide(x, y, cell, replacement)) {}
		else if (cell.Props & CellProperties::MOVE_SIDE      && MoveSide    (x, y, cell, replacement)) {}
		else if (cell.Props & CellProperties::MOVE_FORWARD   && MoveForward (x, y, cell, replacement, hit, hitx, hity)) {}
		else {
			moved = false;
		}
		
		if (moved) {
			SetCell(x, y, Cell::GetDefault(CellType::EMPTY));
		}

		if (!hit) {
			continue;
		}

		if		(cell.Props & CellProperties::HIT_LIKE_BEAM)	   { HitLikeBeam(x, y, hitx, hity, cell); }
		else if (cell.Props & CellProperties::HIT_LIKE_PROJECTILE) { HitLikeProj(x, y, hitx, hity, cell); }
	}
}

bool SandWorker::MoveDown(
	int x, int y,
	const Cell& cell,
	const Cell& replace)
{
	int downX = x + cell.dX;
	int downY = y + cell.dY;

	bool down = IsEmpty(downX, downY);
	if (down) {
		SetCell(downX, downY, replace);
	}

	return down;
}

bool SandWorker::MoveDownSide(
	int x, int y,
	const Cell& cell,
	const Cell& replace)
{
	int downLeftX = x + cell.dY + cell.dX;
	int downLeftY = y - cell.dX + cell.dY;

	int downRightX = x - cell.dY + cell.dX;
	int downRightY = y + cell.dX + cell.dY;

	bool downLeft  = IsEmpty(downLeftX,  downLeftY);
	bool downRight = IsEmpty(downRightX, downRightY);

	if (downLeft && downRight) {
		bool rand = iw::randf() > 0;
		downLeft  = rand ? true  : false;
		downRight = rand ? false : true;
	}

	if		(downLeft)	SetCell(downLeftX,  downLeftY,  replace);
	else if (downRight) SetCell(downRightX, downRightY, replace);

	return downLeft || downRight;
}

bool SandWorker::MoveSide(
	int x, int y,
	const Cell& cell,
	const Cell& replace)
{
	int leftX = x + cell.dY + cell.dX;
	int leftY = y - cell.dX;

	int rightX = x - cell.dY + cell.dX;
	int rightY = y + cell.dX;

	bool left  = IsEmpty(leftX,  leftY);
	bool right = IsEmpty(rightX, rightY);

	if (left && right) {
		bool rand = iw::randf() > 0;
		left  = rand ? true  : false;
		right = rand ? false : true;
	}

	if		(left)	SetCell(leftX,	leftY,	replace);
	else if (right) SetCell(rightX, rightY, replace);

	return left || right;
}

bool SandWorker::MoveForward(
	int x, int y,
	const Cell& cell,
	const Cell& replace,
	bool& hit,
	int& hitx, int& hity)
{
	float life = cell.Life - iw::DeltaTime();

	if (life < 0) {
		SetCell(x, y, Cell::GetDefault(CellType::EMPTY));
	}

	else {
		std::vector<iw::vector2> cellpos = iw::FillLine(x, y, x + cell.dX * iw::DeltaTime(), y + cell.dY * iw::DeltaTime());
		for (int i = 0; i < cellpos.size(); i++) {
			float destX = cellpos[i].x;
			float destY = cellpos[i].y;

			bool forward = IsEmpty(destX, destY);

			if (forward) {
				bool last = i == (cellpos.size() - 1);

				Cell replacement = replace;
				replacement.TileId = cell.TileId;
				replacement.dX = last ? cell.dX : 0;
				replacement.dY = last ? cell.dY : 0;

				SetCell(destX, destY, replacement);
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

		Cell replacement = cell;
		replacement.Life = life;
		replacement.dX = 0;
		replacement.dY = 0;

		SetCell(x, y, replacement);
	}

	return true;
}

void SandWorker::HitLikeProj(
	int x,  int y,
	int lx, int ly,
	const Cell& bullet)
{
	float speed = bullet.Speed();

	Cell hit = GetCell(x, y);
	hit.Life -= speed;

	if (hit.Life < 0) {
		Cell cell = bullet;
		cell.dX += iw::clamp<int>(iw::randi(100) - 50, cell.dX * .8, cell.dX * 1.2);
		cell.dY += iw::clamp<int>(iw::randi(100) - 50, cell.dY * .8, cell.dY * 1.2);
		cell.Life *= 1 - iw::DeltaTime() * 5;

		SetCell(x, y, cell);
	}

	else {
		SetCell(x, y, hit);
	}
}

void SandWorker::HitLikeBeam(
	int x,  int y,
	int lx, int ly,
	const Cell& laser)
{
	float speed = laser.Speed();

	if (speed == 0) {
		return;
	}

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
		cell.dX += iw::clamp<int>(iw::randi(100) - 50, cell.dX * .8, cell.dX * 1.2);
		cell.dY += iw::clamp<int>(iw::randi(100) - 50, cell.dY * .8, cell.dY * 1.2);
		cell.Life *= 1 - iw::DeltaTime() * 5;

		SetCell(x, y, cell);
	}

	else {
		SetCell(x, y, hit);
	}
}

//for (int i = x - 5; i < x + 5; i++)
//for (int j = y - 5; j < y + 5; j++) {
//	if (   inBounds(i, j)
//		&& getCell(i, j).Type != cell.Type)
//	{
//		if (iw::randf() > .5) {
//			setCell(i, j, _DEBRIS);
//		}
//
//		else {
//			setCell(i, j, _EMPTY);
//		}
//	}
//}
