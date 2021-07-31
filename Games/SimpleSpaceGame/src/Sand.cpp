#include "Sand.h"

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
		, world(SandWorld(1920, 1080, 2, 6, 2.f))
	{
		srand(time(nullptr));
	}

	int SandLayer::Initialize() {
		if (int err = Layer::Initialize()) {
			return err;
		}

		shader = Asset->Load<Shader>("shaders/SimpleSpaceGame/texture.shader");

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

			for (int x = -300; x < 300; x++)
			for (int y = -200; y < -50; y++)
			{
				world.SetCell(x, y, _SAND);
			}

			for (int x = -(int)texture->Width() / 2; x < (int)texture->Width() / 2; x++)
			for (int y = -100; y < -50; y++) 
			{
				world.SetCell(x, y, _WATER);
			}

			Space->Clear();

			player = Space->CreateEntity<iw::Transform, Tile, Player>();

			player.Set<iw::Transform>();
			player.Set<Tile>(std::vector<glm::vec2> {
				                 glm::vec2(1, 0), glm::vec2(1, 0),
				glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(2, 1), glm::vec2(3, 1),
				glm::vec2(0, 2), glm::vec2(1, 2), glm::vec2(2, 2), glm::vec2(3, 2),
				glm::vec2(0, 3),                                   glm::vec2(3, 3),
				glm::vec2(0, 4),                                   glm::vec2(3, 4)
			});
			player.Set<Player>();

			reset = false;
		}

		spawnEnemy -= iw::DeltaTime();
		if (spawnEnemy < 0) {
			iw::Entity enemy = Space->CreateEntity<iw::Transform, Tile, Enemy2>();
			enemy.Set<iw::Transform>(glm::vec2(texture->Width() * iw::randf(), texture->Height() + 100 * (1+iw::randf())));
			enemy.Set<Tile>(std::vector<glm::vec2> {
				                 glm::vec2(1, 0),                  glm::vec2(3, 0),
				glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(2, 1), glm::vec2(3, 1),
				glm::vec2(0, 2), glm::vec2(1, 2), glm::vec2(2, 2), glm::vec2(3, 2),
				glm::vec2(0, 3),                                   glm::vec2(3, 3),
				glm::vec2(0, 4),                                   glm::vec2(3, 4)
			});
			enemy.Set<Enemy2>(glm::vec2(texture->Width() * .4f * iw::randf(), (texture->Height() - 200) * .4f * (iw::randf() + 1)));

			spawnEnemy = iw::randf() + 1;
		}

		 // camera frustrum, mostly used at the bottom, but needed for screen mouse pos to world pos

		int height = target->Tex(0)->Height();
		int width  = target->Tex(0)->Width();

		int fy = -(int)texture->Height()/2;//player.Find<iw::Transform>()->Position.y - height / 2;
		int fx = -(int)texture->Width() /2;//player.Find<iw::Transform>()->Position.x - width  / 2;
		int fy2 = fy + height;
		int fx2 = fx + width;

		glm::vec2 pos = mousePos / world.m_scale;
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
				glm::vec2 v = a->Locations[i];
				v.x += int(t->Position.x);
				v.y += int(t->Position.y);

				if (!world.InBounds(v.x, v.y)) continue;
				
				const Cell& cell = world.GetCell(v.x, v.y);

				if (   cell.Type   == CellType::EMPTY
					|| cell.TileId == a->TileId)
				{
					Cell me = _METAL;
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
				&& p->FireButtons != glm::vec2(0.f))
			{
				p->FireTimeout = p->FireButtons.x == 1 ? .1f : .001f;

				float speed      = p->FireButtons.x == 1 ?    1500 :   2500;
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

				glm::vec2 target = player.Find<iw::Transform>()->Position;
			
				float speed = 500;
				const Cell& shot = _LASER;

				Fire(t->Position, target, speed, shot, a->TileId);
			}
		});

		// Sand update

		//stepTimer -= iw::DeltaTime();
		//if (stepTimer < 0 && Keyboard::KeyDown(V)/**/) {
		//	stepTimer = 1 / 20.0f;

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

			//LOG_INFO << "Need to update " << chunkCount << " chunks. dt is " << iw::DeltaTime() << " seconds (" << 1 / iw::DeltaTime() << "fps)";

			std::unique_lock lock(chunkCountMutex);
			chunkCountCV.wait(lock, [&](){ return chunkCount == 0; });
		//}

		// Swap buffers

		target->Tex(0)->Clear();
		unsigned int* colors = (unsigned int*)target->Tex(0)->Colors();

		for (SandChunk* chunk : world.GetVisibleChunks(fx, fy, fx2, fy2)) {
			int startY = iw::clamp(fy  - chunk->m_y, 0, chunk->m_height);
			int startX = iw::clamp(fx  - chunk->m_x, 0, chunk->m_width);
			int endy   = iw::clamp(fy2 - chunk->m_y, 0, chunk->m_height);
			int endx   = iw::clamp(fx2 - chunk->m_x, 0, chunk->m_width);

			for (int y = startY; y < endy; y++)
			for (int x = startX; x < endx; x++) {
				int texi = (chunk->m_x + x - fx) + (chunk->m_y + y - fy) * width;

				colors[texi] = chunk->GetCellUnsafe(x, y).Color.to32(); // assign rgba at the same time

				//if (x == 0 || y == 0) {
				//	colors[texi] = iw::Color(1, 0, 0, 1);
				//}

	/*			if (Keyboard::KeyDown(K)) {
					if (x == 0 || y == 0) {
						colors[texi] = iw::Color(1, 0, 0, 1);
					}
				}*/
			}
		}

		Space->Query<iw::Transform, Tile>().Each([&](
			auto e,
			auto t,
			auto a)
		{
			for (int i = 0; i < a->Locations.size(); i++) {
				glm::vec2 v = a->Locations[i];
				v.x += int(t->Position.x);
				v.y += int(t->Position.y);

				if (!world.InBounds(v.x, v.y)) continue;
				
				const Cell& cell = world.GetCell(v.x, v.y);

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
		
		Space->Query<iw::Transform, Player>().Each([&](
			auto,
			auto t,
			auto p)
		{
			t->Position.x += p->Movement.x * 60 * world.m_scale * iw::DeltaTime();
			t->Position.y += p->Movement.y * 60 * world.m_scale * iw::DeltaTime();
		});

		Space->Query<iw::Transform, Enemy2>().Each([&](
			auto e,
			auto t,
			auto p)
		{
			t->Position = iw::lerp<glm::vec3>(
				t->Position,
				glm::vec3(
					p->Spot.x + cos(iw::TotalTime() + e.Index / 5) * 100,
					p->Spot.y + sin(iw::TotalTime() + e.Index / 5) * 100,
					0.f
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

	std::vector<glm::ivec2> FillLineInGrid(
	int x,  int y,
	int x2, int y2)
	{
		std::vector<glm::ivec2> positions; // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

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

	std::vector<glm::vec2> FillLine(
		float x,  float y, 
		float x2, float y2)
	{
		std::vector<glm::vec2> positions;

		float dx = x2 - x;
		float dy = y2 - y;

		float distance = sqrt(dx*dx + dy*dy);
		dx /= distance;
		dy /= distance;

		for (int i = 0; i < ceil(distance); i++)
		{
			positions.emplace_back(x, y);

			x += dx;
			y += dy;
		}

		return positions;
	}
}

void SandWorker::UpdateChunk() {
	for (int x = m_chunk.m_width  - 1; x >= 0 ; x--)
	for (int y = m_chunk.m_height - 1; y >= 0 ; y--) 
	{
		if (CellAlreadyUpdated(x, y)) continue;

		Cell cell = m_chunk.GetCell(x, y);

		// Would go in grav worker or something

		//if (cell.Gravitised) {
		//	glm::vec2 dir = gravPos - glm::vec2(x, y);

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

		//	cell.Direction  = iw::clamp<glm::vec2>(cell.Direction, -5, 5);
		//	scell.Direction = iw::clamp<glm::vec2>(scell.Direction, -5, 5);
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
	const Cell& cell,
	const Cell& replacement)
{
	int downX = x + cell.dx;
	int downY = y + cell.dy;

	bool downRight, downLeft, down = IsEmpty(downX, downY);
	if (down) {
		SetCell(downX, downY, replacement);
	}

	else {
		int downLeftX = x + cell.dy + cell.dx;
		int downLeftY = y - cell.dx + cell.dy;

		int downRightX = x - cell.dy + cell.dx;
		int downRightY = y + cell.dx + cell.dy;

		downLeft  = IsEmpty(downLeftX, downLeftY);
		downRight = IsEmpty(downRightX, downRightY);

		if (downLeft && downRight) {
			bool rand = iw::randf() > 0;
			downLeft  = rand ? true  : false;
			downRight = rand ? false : true;
		}

		if (downLeft) {
			SetCell(downLeftX, downLeftY, replacement);
		}

		else if (downRight) {
			SetCell(downRightX, downRightY, replacement);
		}
	}

	if (down || downLeft || downRight) {
		SetCell(x, y, _EMPTY);
	}
}

void SandWorker::MoveLikeWater(
	int x, int y,
	const Cell& cell,
	const Cell& replacement)
{
	int downX = x + cell.dx;
	int downY = y + cell.dy;

	bool right, left, down = IsEmpty(downX, downY);
	if (down) {
		SetCell(downX, downY, replacement);
	}

	else {
		int leftX = x + cell.dy + cell.dx;
		int leftY = y - cell.dx;

		int rightX = x - cell.dy + cell.dx;
		int rightY = y + cell.dx;

		left  = IsEmpty(leftX,  leftY);
		right = IsEmpty(rightX, rightY);

		if (left && right) {
			bool rand = iw::randf() > 0;
			left  = rand ? true  : false;
			right = rand ? false : true;
		}

		if (left) {
			SetCell(leftX, leftY, replacement);
		}

		else if (right) {
			SetCell(rightX, rightY, replacement);
		}
	}

	if (down || left || right) {
		SetCell(x, y, _EMPTY);
	}
}

std::pair<bool, glm::vec2> SandWorker::MoveForward(
	int x, int y,
	const Cell& cell,
	const Cell& replacement)
{
	int px = floor(cell.x);
	int py = floor(cell.y);

	int nx = floor(cell.x + cell.dx * iw::DeltaTime());
	int ny = floor(cell.y + cell.dy * iw::DeltaTime());

	bool hasHit = false;
	glm::vec2 hitLocation;

	float life = cell.Life - iw::DeltaTime();

	if (life < 0) {
		SetCell(x, y, _EMPTY);
	}

	// need to get off grid, cant shoot in arbitrary floating direction

	else {
		std::vector<glm::ivec2> cellpos = iw::FillLineInGrid(px, py, nx, ny);
		for (int i = 0; i < cellpos.size(); i++) {
			float destX = cellpos[i].x;
			float destY = cellpos[i].y;

			bool forward = IsEmpty(destX, destY);

			if (forward) {
				bool last = i == (cellpos.size() - 1);

				Cell replace = replacement;
				replace.TileId = cell.TileId;
				replace.dx = last ? cell.dx : 0;
				replace.dy = last ? cell.dy : 0;

				SetCell(destX, destY, replace);
			}

			else if (!hasHit && InBounds(destX, destY)) {
				Cell hit = GetCell(destX, destY);
				if (   hit.Type   != cell.Type
					&& hit.TileId != cell.TileId)
				{
					hasHit = true;
					hitLocation = glm::vec2(destX, destY);
				}

				if (hasHit) {
					break;
				}
			}
		}

		Cell replace = cell;
		replace.Life = life;
		
		SetCell(px, py, replace);
	}

	return { hasHit, hitLocation };
}

void SandWorker::HitLikeBullet(
	int x, int y,
	const Cell& bullet)
{
	Cell hit = GetCell(x, y);

	hit.Life -= bullet.Speed();
	if (hit.Life < 0) {
		Cell cell = bullet;
		cell.dx += iw::randi(600) - 300;
		cell.dy += iw::randi(600) - 300;
		cell.Life *= .95f;

		SetCell(x, y, cell);
	}

	else {
		SetCell(x, y, hit);
	}
}

void SandWorker::HitLikeLaser(
	int x, int y,
	const Cell& laser)
{
	Cell hit = GetCell(x, y);

	hit.Life -= laser.Speed();
	if (hit.Life <= 0) {
		Cell cell = laser;
		cell.dx += iw::randi(600) - 300;
		cell.dy += iw::randi(600) - 300;
		cell.Life /= 2;

		SetCell(x, y, cell);

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

	else {
		SetCell(x, y, hit);
	}
}
