#include "Layers/SandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

#include <numeric>

namespace iw {
	SandLayer::SandLayer()
		 : Layer("Ray Marching")
		, shader(nullptr)
		, target (nullptr)
		, world(SandWorld(192, 1080, 25.f))
	{
		srand(time(nullptr));
	}

	int SandLayer::Initialize() {
		if (int err = Layer::Initialize()) {
			return err;
		}

		shader = Asset->Load<Shader>("shaders/texture.shader");

		iw::ref<iw::Texture> texture = REF<iw::Texture>(
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

		if (reset) {
			world.m_chunks.find(0)->second.Reset();

			//for (int x = 0; x < world.m_chunkWidth; x++)
			//for (int y = 0; y < world.m_chunkHeight - 300; y++) {
			//	world.m_chunks.find(0)->second.SetCell(x, y, _SAND);
			//}

			Space->Clear();

			player = Space->CreateEntity<iw::Transform, Tile, Player>();

			player.Set<iw::Transform>(iw::vector2(world.m_chunkWidth / 2, world.m_chunkHeight / 2 + 100));
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
		//	enemy.Set<iw::Transform>(iw::vector2(world.m_chunkWidth * iw::randf(), world.m_chunkHeight + 100 * iw::randf()));
		//	enemy.Set<Tile>(std::vector<iw::vector2> {
		//		vector2(1, 0), vector2(3, 0),
		//			vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
		//			vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
		//			vector2(0, 3), vector2(3, 3),
		//			vector2(0, 4), vector2(3, 4)
		//	});
		//	enemy.Set<Enemy2>(iw::vector2(world.m_chunkWidth * .4f * (iw::randf() + 1), 200 + (world.m_chunkHeight - 200) * .4f * (iw::randf() + 1)));

		//	spawnEnemy = iw::randf() + 1;
		//}

		vector2 pos = mousePos / world.m_scale;
		pos.x = floor(pos.x);
		pos.y = floor(world.m_chunkHeight - pos.y); // find correct valuer
		
		gravPos = pos;

		if(Keyboard::KeyDown(iw::E) || Keyboard::KeyDown(iw::C) || Keyboard::KeyDown(iw::F) || Keyboard::KeyDown(iw::R)) {
			for (int i = 0; i < 1; i++) {
			for (int ii = 0; ii < 1; ii++) {
				if (Keyboard::KeyDown(iw::C)) {
					world.SetCell(pos.x + i, pos.y + ii, _SAND);
				}

				else if (Keyboard::KeyDown(iw::F)) {
					world.SetCell(pos.x + i, pos.y + ii, _WATER);
				}

				else if (Keyboard::KeyDown(iw::E)) {
					world.SetCell(pos.x + i, pos.y + ii, _EMPTY);
				}

				else if (Keyboard::KeyDown(iw::R)) {
					world.SetCell(pos.x + i, pos.y + ii, _ROCK);
				}
			}
			}
		}

		//Space->Query<iw::Transform, Tile>().Each([&](
		//	auto e,
		//	auto t,
		//	auto a)
		//{
		//	if (a->Locations.size() < a->InitialLocationsSize / 3) {
		//		if ((*Space).HasComponent<Player>(e)) {
		//			reset = true;
		//		}

		//		(*Space).QueueEntity(e, iw::func_Destroy);
		//		return;
		//	}

		//	for (int i = 0; i < a->Locations.size(); i++) {
		//		vector2 v = a->Locations[i];
		//		v.x += t->Position.x;
		//		v.y += t->Position.y;

		//		if (!world.InBounds(v.x, v.y)) continue;
		//		
		//		Cell& cell = world.GetCell(v.x, v.y);

		//		if (   cell.Type == CellType::EMPTY
		//			|| cell.TileId == a->TileId)
		//		{
		//			world.SetCell(v.x, v.y, _METAL);
		//		}

		//		else {
		//			a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
		//			i--;
		//		}
		//	}

		//	for (vector2 v : a->Locations) {
		//		v.x += t->Position.x;
		//		v.y += t->Position.y;

		//		Cell* c = world.SetCell(v.x, v.y,  _METAL);
		//		if (c) {
		//			c->TileId = a->TileId;
		//		}
		//	}
		//});

		//Space->Query<iw::Transform, Tile, Player>().Each([&](
		//	auto,
		//	auto t,
		//	auto a,
		//	auto p)
		//{
		//	p->FireTimeout -= iw::DeltaTime();

		//	if (   p->FireTimeout < 0
		//		&& p->FireButtons != 0)
		//	{
		//		p->FireTimeout = p->FireButtons.x == 1 ? .05f : .001f;

		//		float speed      = p->FireButtons.x == 1 ?      10 :     /*2*/5;
		//		const Cell& shot = p->FireButtons.x == 1 ? _BULLET : _LASER;

		//		Fire(t->Position, pos, speed, shot, a->TileId);
		//	}
		//});

		//Space->Query<iw::Transform, Tile, Enemy2>().Each([&](
		//	auto,
		//	auto t,
		//	auto a,
		//	auto p)
		//{
		//	p->FireTimeout -= iw::DeltaTime();

		//	if (p->FireTimeout < 0) {
		//		p->FireTimeout = (iw::randf() + 2) * .5f;

		//		vector2 target = player.Find<iw::Transform>()->Position;
		//	
		//		float speed = 5;
		//		const Cell& shot = _LASER;

		//		Fire(t->Position, target, speed, shot, a->TileId);
		//	}
		//});

		//world.CommitAdditions();

		// Sand update

		stepTimer -= iw::DeltaTime();
		if (stepTimer < 0 && Keyboard::KeyDown(V)) {
			stepTimer = .05;

			for(auto& [location, chunk] : world.m_chunks) {
				for (int x = 0; x < chunk.m_width;  x++)
				for (int y = 0; y < chunk.m_height; y++) {
					Cell& cell = chunk.GetCell(x, y);

					if (cell.Gravitised) {
						vector2 dir = gravPos - vector2(x, y);

						float div = 2;
						while (abs((dir / div).major()) > 1) {
							dir /= div;
							div = iw::clamp(div - iw::randf(), 1.2f, 2.f);
						}

						Cell& scell = chunk.GetCell(x, y, true);

						cell.Direction  += dir + iw::DeltaTime();
						scell.Direction += dir + iw::DeltaTime();

						cell.Direction  *= .9f;
						scell.Direction *= .9f;

						cell.Direction  = iw::clamp<vector2>(cell.Direction, -5, 5);
						scell.Direction = iw::clamp<vector2>(scell.Direction, -5, 5);
					}

					// X and y need to be in world coords now

					int worldX = x + location.x * chunk.m_width;
					int worldY = y + location.y * chunk.m_height;

					switch (cell.Type) {
						case CellType::SAND: {
							MoveLikeSand(worldX, worldY, cell, cell);
							break;
						}
						case CellType::WATER: {
							MoveLikeWater(worldX, worldY, cell, _WATER);
							break;
						}
						case CellType::BULLET: {
							auto result = MoveForward(worldX, worldY, cell, _BULLET);
							if (result.first) {
								HitLikeBullet(result.second.x, result.second.y, cell);
							}
							break;
						}
						case CellType::LASER: {
							auto result = MoveForward(worldX, worldY, cell, _LASER);
							if (result.first) {
								HitLikeLaser(result.second.x, result.second.y, cell);
							}
							break;
						}
					}
				}
			}

			world.CommitAdditions();
		}

		// Swap buffers

		// Get chunks that need rendering and only render those to a single texture

		int x  = 0; // camera frustrum
		int y  = 0;
		int x2 = target->Tex(0)->Width();
		int y2 = target->Tex(0)->Height();

		unsigned int* colors = (unsigned int*)target->Tex(0)->Colors();

		for (auto [location, chunk] : world.GetVisibleChunks(x, y, x2, y2)) {
			int minX = location.x * chunk->m_width;
			int minY = location.y * chunk->m_height;

			for (int y = 0; y < chunk->m_height; y++) {
			for (int x = 0; x < chunk->m_width;  x++) {
				int texi = (minX + x) + (minY + y) * target->Tex(0)->Width();

				colors[texi] = chunk->GetCell(x, y).Color; // assign rgba at the same time

				if (Keyboard::KeyDown(K)) {
					if (x == chunk->m_width - 1 || y == chunk->m_height - 1) {
						colors[texi] = iw::Color(1, 0, 0, 1);
					}
				}
			}
			}
			//for (int i = 0; i < world.ChunkWidth() * world.ChunkHeight(); i++) {
			//	((unsigned int*)target->Tex(0)->Colors())[i] = chunk->Cells[i].Color; // assign RGBA all at once
			//}
		}

		//Space->Query<iw::Transform, Tile>().Each([&](
		//	auto e,
		//	auto t,
		//	auto a)
		//{
		//	for (int i = 0; i < a->Locations.size(); i++) {
		//		vector2 v = a->Locations[i];
		//		v.x += t->Position.x;
		//		v.y += t->Position.y;

		//		if (!world.InBounds(v.x, v.y)) continue;
		//		
		//		Cell& cell = world.GetCell(v.x, v.y, true);

		//		if (cell.Type == CellType::EMPTY) continue;

		//		if (cell.TileId == a->TileId) {
		//			world.SetCell(v.x, v.y, _EMPTY);
		//		}

		//		else {
		//			a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
		//			i--;
		//		}
		//	}
		//});
		//
		//world.CommitAdditions();

		//Space->Query<iw::Transform, Player>().Each([&](
		//	auto,
		//	auto t,
		//	auto p)
		//{
		//	t->Position += (t->Right() * p->Movement.x * 60
		//		+ t->Up() * p->Movement.y * 60)
		//		* iw::DeltaTime() * world.m_scale;
		//});

		//Space->Query<iw::Transform, Enemy2>().Each([&](
		//	auto e,
		//	auto t,
		//	auto p)
		//{
		//	t->Position = iw::lerp<vector2>(
		//		t->Position,
		//		iw::vector2(
		//			p->Spot.x + cos(iw::TotalTime() + e.Index / 5) * 100,
		//			p->Spot.y + sin(iw::TotalTime() + e.Index / 5) * 100
		//		),
		//		iw::DeltaTime());
		//});

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

	void SandLayer::MoveLikeSand(
		int x, int y,
		Cell& cell,
		const Cell& replacement)
	{
		vector2 d = cell.Direction;

		//if (abs(d.major()) < 1) {
		//	LOG_INFO << "Sand with d = 0!";
		//	return;
		//}

		vector2 l = vector2(ceil ( d.y + d.x), floor(-d.x + d.y));
		vector2 r = vector2(floor(-d.y + d.x), ceil ( d.x + d.y));

		bool down = world.IsEmpty(x + int(d.x), y + int(d.y), true);
		bool left = world.IsEmpty(x + int(l.x), y + int(l.y), true);
		bool rght = world.IsEmpty(x + int(r.x), y + int(r.y), true);

		if (down || left || rght) {
			world.SetCell(x, y, _EMPTY);
		}

		if      (down) world.SetCell(x + int(d.x), y + int(d.y), replacement);
		else if (left) world.SetCell(x + int(l.x), y + int(l.y), replacement);
		else if (rght) world.SetCell(x + int(r.x), y + int(r.y), replacement);
	}

	void SandLayer::MoveLikeWater(
		int x, int y,
		const Cell& cell,
		const Cell& replacement)
	{
		bool down     = world.IsEmpty(x,     y - 1, true);
		bool left     = world.IsEmpty(x - 1, y,     true);
		bool rght     = world.IsEmpty(x + 1, y,     true);

		if (down || left || rght) {
			world.SetCell(x, y, _EMPTY);
		}

		if      (down) world.SetCell(x,     y - 1, replacement);
		else if (left) world.SetCell(x - 1, y,     replacement);
		else if (rght) world.SetCell(x + 1, y,     replacement);
	}

	std::pair<bool, iw::vector2> SandLayer::MoveForward(
		int x, int y,
		Cell& cell,
		const Cell& replacement)
	{
		bool hasHit = false;
		iw::vector2 hitLocation;

		if (cell.Life < 0) {
			world.SetCell(x, y, _EMPTY);
		}

		else {
			std::vector<iw::vector2> cellpos = FillLine(
				floor(x), floor(y),
				ceil(x + cell.Direction.x), ceil(y + cell.Direction.y)
			);

			for (int i = 0; i < cellpos.size(); i++) {
				float destX = cellpos[i].x;
				float destY = cellpos[i].y;

				bool forward = world.IsEmpty(destX, destY, true);

				if (forward) {
					Cell* c = world.SetCell(destX, destY, replacement);
					if (c) {
						if (i == cellpos.size() - 1) {
							c->Direction = cell.Direction;
						}

						else {
							c->Direction = 0;
						}

						c->TileId = cell.TileId;
					}
				}

				else if (!hasHit && world.InBounds(destX, destY)) {
					Cell& hit = world.GetCell(destX, destY, true);
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

	void SandLayer::HitLikeBullet(
		int x, int y,
		Cell& cell)
	{
		Cell& hit = world.GetCell(x, y);

		hit.Life -= cell.Direction.length() * 2;
		if (hit.Life < 0) {
			Cell* c = world.SetCell(x, y, cell);
			if (c) {
				float l = c->Direction.length();

				c->Direction += iw::vector2(iw::randf(), iw::randf()) * l;
				c->Direction.normalize();
				c->Direction *= l;

				c->Life = cell.Life / 2;
			}
		}
	}
	
	void SandLayer::HitLikeLaser(
		int x, int y,
		Cell& cell)
	{
		Cell& hit = world.GetCell(x, y);

		hit.Life -= cell.Direction.length() * 2;
		if (hit.Life <= 0) {
			Cell* c = world.SetCell(x, y, cell);
			if (c) {
				float l = c->Direction.length();

				c->Direction += iw::vector2(iw::randf(), iw::randf()) * l / 2;
				c->Direction.normalize();
				c->Direction *= l;

				c->Life = cell.Life / 2;
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

	std::vector<iw::vector2> SandLayer::FillLine(
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

