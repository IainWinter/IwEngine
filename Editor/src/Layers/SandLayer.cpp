#include "Layers/SandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

const Cell _EMPTY = { CellType::EMPTY,  iw::Color(0), 0, 0 };

const Cell _SAND = { CellType::SAND,   iw::Color::From255(237, 201, 175), 1 };
const Cell _WATER = { CellType::WATER,  iw::Color::From255(175, 201, 237) };
const Cell _ROCK = { CellType::ROCK,   iw::Color::From255(201, 201, 201) };
const Cell _METAL = { CellType::METAL,  iw::Color::From255(230, 230, 230), 10 };
const Cell _DEBRIS = { CellType::DEBRIS, iw::Color::From255(150, 150, 150), 1 };

const Cell _LASER = { CellType::LASER,  iw::Color::From255(255,   0,   0), .06f };
const Cell _BULLET = { CellType::BULLET, iw::Color::From255(255,   255, 0), .02f };

namespace iw {
	SandLayer::SandLayer()
		 : Layer("Ray Marching")
		, shader(nullptr)
		, target (nullptr)
		, world(SandWorld(1920*2, 1080*2, 10, 10, 2.f))
	{
		srand(time(nullptr));
	}

	int SandLayer::Initialize() {
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
		LOG_INFO << iw::DeltaTime();

		// Game update

		if (reset) {
			world.Reset();

			for (int x = 0; x < texture->Width(); x++)
			for (int y = 0; y < texture->Height() / 3; y++) {
				world.SetCell(x, y, _METAL);
			}

			Space->Clear();

			player = Space->CreateEntity<iw::Transform, Tile, Player>();

			player.Set<iw::Transform>(iw::vector2(texture->Width() / 2, texture->Height() / 2 + 100));
			player.Set<Tile>(std::vector<iw::vector2> {
				vector2(1, 0), vector2(1, 0),
					vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
					vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
					vector2(0, 3), vector2(3, 3),
					vector2(0, 4), vector2(3, 4)
			});
			player.Set<Player>();

			reset = false;
		}

		//spawnEnemy -= iw::DeltaTime();
		//if (spawnEnemy < 0) {
		//	iw::Entity enemy = Space->CreateEntity<iw::Transform, Tile, Enemy2>();
		//	enemy.Set<iw::Transform>(iw::vector2(texture->Width() * iw::randf(), texture->Height() + 100 * iw::randf()));
		//	enemy.Set<Tile>(std::vector<iw::vector2> {
		//		vector2(1, 0), vector2(3, 0),
		//			vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
		//			vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
		//			vector2(0, 3), vector2(3, 3),
		//			vector2(0, 4), vector2(3, 4)
		//	});
		//	enemy.Set<Enemy2>(iw::vector2(texture->Width() * .4f * (iw::randf() + 1), 200 + (texture->Height() - 200) * .4f * (iw::randf() + 1)));

		//	spawnEnemy = iw::randf() + 1;
		//}

		 // camera frustrum, mostly used at the bottom, but needed for screen mouse pos to world pos

		int height = target->Tex(0)->Height();
		int width  = target->Tex(0)->Width();

		int fy = player.Find<iw::Transform>()->Position.y - height / 2;
		int fx = player.Find<iw::Transform>()->Position.x - width  / 2;
		int fy2 = fy + height;
		int fx2 = fx + width;

		vector2 pos = mousePos / world.m_scale;
		pos.x = floor(pos.x) + fx;
		pos.y = floor(texture->Height() - pos.y) + fy; // find correct valuer
		
		gravPos = pos;

		if(Keyboard::KeyDown(iw::E) || Keyboard::KeyDown(iw::C) || Keyboard::KeyDown(iw::F) || Keyboard::KeyDown(iw::R)) {
			Cell type;
			if (Keyboard::KeyDown(iw::C)) {
				type = _SAND;
			}

			else if (Keyboard::KeyDown(iw::F)) {
				type = _WATER;
			}

			else if (Keyboard::KeyDown(iw::E)) {
				type = _EMPTY;
			}

			else if (Keyboard::KeyDown(iw::R)) {
				type = _ROCK;
			}
			
			for (int i = 0; i < 20; i++) {
			for (int ii = 0; ii < 20; ii++) {
				world.SetCell(pos.x + i, pos.y + ii, type);
			}
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
				vector2 v = a->Locations[i];
				v.x += t->Position.x;
				v.y += t->Position.y;

				if (!world.InBounds(v.x, v.y)) continue;
				
				Cell& cell = world.GetCell(v.x, v.y);

				if (   cell.Type == CellType::EMPTY
					|| cell.TileId == a->TileId)
				{
					world.SetCell(v.x, v.y, _METAL);
				}

				else {
					a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
					i--;
				}
			}

			for (vector2 v : a->Locations) {
				v.x += t->Position.x;
				v.y += t->Position.y;

				Cell* c = world.SetCell(v.x, v.y,  _METAL);
				if (c) {
					c->TileId = a->TileId;
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
				p->FireTimeout = p->FireButtons.x == 1 ? .1f : .001f;

				float speed      = p->FireButtons.x == 1 ?      10 :     25;
				const Cell& shot = p->FireButtons.x == 1 ? _BULLET : _LASER;

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
				p->FireTimeout = (iw::randf() + 2) * .5f;

				vector2 target = player.Find<iw::Transform>()->Position;
			
				float speed = 5;
				const Cell& shot = _LASER;

				Fire(t->Position, target, speed, shot, a->TileId);
			}
		});

		world.CommitAdditions();

		// Sand update

		//stepTimer -= iw::DeltaTime();
		//if (stepTimer < 0 /*&& Keyboard::KeyDown(V)*/) {
			//stepTimer = 1 / 60.0f;

			std::mutex chunkCountMutex;
			std::condition_variable chunkCountCV;
			int chunkCount = 0;

			for(auto& pair: world.m_chunks) {
				if (pair.second.IsEmpty()) {
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

			std::unique_lock lock(chunkCountMutex);
			chunkCountCV.wait(lock, [&](){ return chunkCount == 0; });

			world.CommitAdditions();
		//}

		// Swap buffers

		target->Tex(0)->Clear();
		unsigned int* colors = (unsigned int*)target->Tex(0)->Colors();

		for (SandChunk* chunk : world.GetVisibleChunks(fx, fy, fx2, fy2)) {
			int startY = iw::clamp(fy - chunk->m_y, 0, chunk->m_height);
			int startX = iw::clamp(fx - chunk->m_x, 0, chunk->m_width);

			int endY = chunk->m_height;
			int endX = chunk->m_width;
			if (chunk->m_y + chunk->m_height - fy > height) { // is there a way to do this with clamps?
				endY += fy; // add cus this is neg 
			}
			if (chunk->m_x + chunk->m_width - fx > width) {
				endX += fx;
			}

			for (int y = startY; y < endY; y++)
			for (int x = startX; x < endX; x++) {
				int texi = (chunk->m_x + x - fx) + (chunk->m_y + y - fy) * width;

				colors[texi] = chunk->GetCell(x, y).Color; // assign rgba at the same time

				if (x == 0 || y == 0) {
					colors[texi] = iw::Color(1, 0, 0, 1);
				}

				//if (Keyboard::KeyDown(K)) {
				//	if (x == 0 || y == 0) {
				//		colors[texi] = iw::Color(1, 0, 0, 1);
				//	}
				//}
			}

			//for (int i = 0; i < world.ChunkWidth() * world.ChunkHeight(); i++) {
			//	((unsigned int*)target->Tex(0)->Colors())[i] = chunk->Cells[i].Color; // assign RGBA all at once
			//}
		}

		Space->Query<iw::Transform, Tile>().Each([&](
			auto e,
			auto t,
			auto a)
		{
			for (int i = 0; i < a->Locations.size(); i++) {
				vector2 v = a->Locations[i];
				v.x += t->Position.x;
				v.y += t->Position.y;

				if (!world.InBounds(v.x, v.y)) continue;
				
				Cell& cell = world.GetCell(v.x, v.y, true);

				if (cell.Type == CellType::EMPTY) continue;

				if (cell.TileId == a->TileId) {
					world.SetCell(v.x, v.y, _EMPTY);
				}

				else {
					a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
					i--;
				}
			}
		});
		
		world.CommitAdditions();

		Space->Query<iw::Transform, Player>().Each([&](
			auto,
			auto t,
			auto p)
		{
			t->Position += (t->Right() * p->Movement.x * 60
				+ t->Up() * p->Movement.y * 60)
				* iw::DeltaTime() * world.m_scale;
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
	for (int x = 0; x < m_chunk.m_width; x++)
	for (int y = 0; y < m_chunk.m_height; y++) {
		Cell& cell = m_chunk.GetCell(x, y);

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

		switch (cell.Type) {
			case CellType::SAND: {
				MoveLikeSand(x, y, cell, cell);
				break;
			}
			case CellType::WATER: {
				MoveLikeWater(x, y, cell, _WATER);
				break;
			}
			case CellType::BULLET: {
				auto result = MoveForward(x, y, cell, _BULLET);
				if (result.first) {
					HitLikeBullet(result.second.x, result.second.y, cell);
				}
				break;
			}
			case CellType::LASER: {
				auto result = MoveForward(x, y, cell, _LASER);
				if (result.first) {
					HitLikeLaser(result.second.x, result.second.y, cell);
				}
				break;
			}
		}
	}
}

void SandWorker::MoveLikeSand(
	int x, int y,
	Cell& cell,
	const Cell& replacement)
{
	int px = x;//cell.Pos.x;
	int py = y;//cell.Pos.y;

	int dX = cell.dX;//cell.Vel.x;
	int dY = cell.dY;//cell.Vel.y;

	int downX = px + dX;
	int downY = py + dY;

	int downLeftX = px + dY + dX;
	int downLeftY = py - dX + dY;

	int downRightX = px - dY + dX;
	int downRightY = py + dX + dY;

	bool moved = true;

	if		(IsEmpty(downX,      downY,      true)) SetCell(downX,      downY,      replacement);
	else if (IsEmpty(downLeftX,  downLeftY,  true)) SetCell(downLeftX,  downLeftY,  replacement);
	else if (IsEmpty(downRightX, downRightY, true)) SetCell(downRightX, downRightY, replacement);
	else moved = false;

	//cell.Vel.y -= 1;
	//cell.Pos += cell.Vel;

	if (moved) {
		SetCell(x, y, _EMPTY);
	}

	//else {
	//	cell.Vel.y = -1;
	//	cell.Vel.x = 0;
	//}
}

void SandWorker::MoveLikeWater(
	int x, int y,
	const Cell& cell,
	const Cell& replacement)
{
	int downX = x + cell.dX;
	int downY = y + cell.dY;

	int leftX = x + cell.dY + cell.dX;
	int leftY = y - cell.dX/* + cell.dY*/;

	int rightX = x - cell.dY + cell.dX;
	int rightY = y + cell.dX/* + cell.dY*/;

	bool moved = true;

	if		(IsEmpty(downX,  downY,  true)) SetCell(downX,  downY,  replacement);
	else if (IsEmpty(leftX,  leftY,  true)) SetCell(leftX,  leftY,  replacement);
	else if (IsEmpty(rightX, rightY, true)) SetCell(rightX, rightY, replacement);
	else moved = false;

	if (moved) {
		SetCell(x, y, _EMPTY);
	}

	//bool down = IsEmpty(x,     y - 1, true);
	//bool left = IsEmpty(x - 1, y,     true);
	//bool rght = IsEmpty(x + 1, y,     true);

	//if (down || left || rght) {
	//	SetCell(x, y, _EMPTY);
	//}

	//if		(down) SetCell(x, y - 1, replacement);
	//else if (left) SetCell(x - 1, y, replacement);
	//else if (rght) SetCell(x + 1, y, replacement);
}

std::pair<bool, iw::vector2> SandWorker::MoveForward(
	int x, int y,
	Cell& cell,
	const Cell& replacement)
{
	bool hasHit = false;
	iw::vector2 hitLocation;

	if (cell.Life < 0) {
		SetCell(x, y, _EMPTY);
	}

	else {
		std::vector<iw::vector2> cellpos = iw::FillLine(x, y, x + cell.dX, y + cell.dY);
		for (int i = 0; i < cellpos.size(); i++) {
			float destX = cellpos[i].x;
			float destY = cellpos[i].y;

			bool forward = IsEmpty(destX, destY, true);

			if (forward) {
				Cell* c = SetCell(destX, destY, replacement);
				if (c) {
					if (i == cellpos.size() - 1) {
						c->dX = cell.dX;
						c->dY = cell.dY;
					}

					else {
						c->dY = 0;
						c->dX = 0;
					}

					c->TileId = cell.TileId;
				}
			}

			else if (!hasHit && InBounds(destX, destY)) {
				Cell& hit = GetCell(destX, destY, true);
				if (   hit.Type   != cell.Type
					&& hit.TileId != cell.TileId)
				{
					hasHit = true;
					hitLocation = iw::vector2(destX, destY);
				}

				if (hasHit) {
					break;
				}
			}
		}
	}

	cell.Life -= iw::DeltaTime();

	return { hasHit, hitLocation };
}

void SandWorker::HitLikeBullet(
	int x, int y,
	Cell& cell)
{
	//cell.Life /= 2;

	Cell& hit = GetCell(x, y, true);

	hit.Life -= cell.Speed();
	if (hit.Life < 0) {
		Cell* c = SetCell(x, y, cell);
		if (c) {
			c->dX += iw::randi(6) - 3;
			c->dY += iw::randi(6) - 3;
		}
	}
}

void SandWorker::HitLikeLaser(
	int x, int y,
	Cell& cell)
{
	//cell.Life /= 2;

	Cell& hit = GetCell(x, y, true);

	hit.Life -= cell.Speed();
	if (hit.Life <= 0) {
		Cell* c = SetCell(x, y, cell);
		if (c) {
			c->dX += iw::randi(4) - 2;
			c->dY += iw::randi(4) - 2;
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
	}
}
