#include "Layers/SandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

#include <numeric>

namespace iw {
	SandLayer::SandLayer()
		 : Layer("Ray Marching")
		, shader(nullptr)
		, texture(nullptr)
		, target (nullptr)
		, m_scale(2.5)
	{
		srand(time(nullptr));
	}

	int SandLayer::Initialize() {
		if (int err = Layer::Initialize()) {
			return err;
		}

		shader = Asset->Load<Shader>("shaders/texture.shader");

		texture = REF<Texture>(Renderer->Width()  / m_scale, Renderer->Height() / m_scale, TEX_2D, RGBA);
		texture->SetFilter(NEAREST);
		texture->CreateColors();
		texture->Clear();

		target = REF<RenderTarget>();
		target->AddTexture(texture);

		reset = true;

		return 0;
	}

	void SandLayer::PostUpdate() {
		if (reset) {
			m_cells.clear();
			m_swap .clear();

			m_cells.resize(texture->ColorCount() / texture->Channels());
			m_swap .resize(texture->ColorCount() / texture->Channels());

			for (size_t x = 0; x < texture->Width(); x++)
				for (size_t y = 0; y < texture->Height() - 300; y++) {
					m_swap[getCoords(x, y)] = _SAND;
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


		spawnEnemy -= iw::DeltaTime();
		if (spawnEnemy < 0) {
			iw::Entity enemy = Space->CreateEntity<iw::Transform, Tile, Enemy2>();
			enemy.Set<iw::Transform>(iw::vector2(texture->Width() * iw::randf(), texture->Height() + 100 * iw::randf()));
			enemy.Set<Tile>(std::vector<iw::vector2> {
				vector2(1, 0), vector2(3, 0),
					vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
					vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
					vector2(0, 3), vector2(3, 3),
					vector2(0, 4), vector2(3, 4)
			});
			enemy.Set<Enemy2>(iw::vector2(texture->Width() * .4f * (iw::randf() + 1), 200 + (texture->Height() - 200) * .4f * (iw::randf() + 1)));

			spawnEnemy = iw::randf() + 1;
		}

		gravPos.y = 200;
		gravPos.x = sin(iw::TotalTime() / 10) * texture->Width() / 1.1f;


		//if (player.Find<Player>()->Movement != 0 || player.Find<Player>()->FireButtons != 0) {

		vector2 pos = mousePos / m_scale;
		pos.x = floor(pos.x);
		pos.y = floor(texture->Height() - pos.y);
		
		gravPos = pos;

		if(Keyboard::KeyDown(iw::E) || Keyboard::KeyDown(iw::F) || Keyboard::KeyDown(iw::R)) {
			for (int i = 0; i < 20; i++) {
			for (int ii = 0; ii < 20; ii++) {
				if (inBounds(pos.x + i, pos.y + ii)) {
					size_t index = getCoords(pos.x + i, pos.y + ii);

					if (Keyboard::KeyDown(iw::E)) {
						m_swap[index] = _SAND;
					}

					else if (Keyboard::KeyDown(iw::F)) {
						m_swap[index] = _WATER;
					}

					else if (Keyboard::KeyDown(iw::R)) {
						m_swap[index] = _ROCK;
					}
				}
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

			for (size_t i = 0; i < a->Locations.size(); i++) {
				vector2 v = a->Locations[i];
				v.x += t->Position.x;
				v.y += t->Position.y;

				if (!inBounds(v.x, v.y)) continue;
				
				Cell& cell = getCell(v.x, v.y);

				if (   cell.Type == CellType::EMPTY
					|| cell.TileId == a->TileId)
				{
					setCell(v.x, v.y, _METAL);
				}

				else {
					a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
					i--;
				}
			}

			for (vector2 v : a->Locations) {
				v.x += t->Position.x;
				v.y += t->Position.y;

				Cell* c = setCell(v.x, v.y, _METAL);
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
				p->FireTimeout = p->FireButtons.x == 1 ? .05f : .001f;

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

		for (size_t x = 0; x < texture->Width();  x++)
		for (size_t y = 0; y < texture->Height(); y++) {
			Cell& cell = getCell(x, y);

			if (cell.Gravitised) {
				vector2 dir = gravPos - vector2(x, y);

				float div = 2;
				while (abs((dir / div).major()) > 1) {
					dir /= div;
					div = iw::clamp(div - iw::randf(), 1.2f, 2.f);
				}

				Cell& scell = getCell(x, y, true);

				cell.Direction += dir + iw::DeltaTime();
				scell.Direction += dir + iw::DeltaTime();

				cell.Direction *= .9f;
				scell.Direction *= .9f;

				cell.Direction  = iw::clamp<vector2>(cell.Direction, -5, 5);
				scell.Direction = iw::clamp<vector2>(scell.Direction, -5, 5);
			}

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

		// Swap buffers
		for (size_t i = 0; i < texture->ColorCount() / texture->Channels(); i++) {
			m_cells[i] = m_swap[i];
			((unsigned int*)texture->Colors())[i] = m_cells[i].Color; // assign RGBA all at once
		}

		Space->Query<iw::Transform, Tile>().Each([&](
			auto e,
			auto t,
			auto a)
		{
			for (size_t i = 0; i < a->Locations.size(); i++) {
				vector2 v = a->Locations[i];
				v.x += t->Position.x;
				v.y += t->Position.y;

				if (!inBounds(v.x, v.y)) continue;
				
				Cell& cell = getCell(v.x, v.y);

				if (cell.Type == CellType::EMPTY) continue;

				if (cell.TileId == a->TileId) {
					setCell(v.x, v.y, _EMPTY);
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
			auto,
			auto p)
		{
			t->Position += (t->Right() * p->Movement.x * 60
				+ t->Up() * p->Movement.y * 60)
				* iw::DeltaTime() * m_scale;
		});

		Space->Query<iw::Transform, Tile, Enemy2>().Each([&](
			auto e,
			auto t,
			auto,
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
		//}

		texture->Update(Renderer->Device); // should be auto in apply filter
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
		size_t x, size_t y,
		Cell& cell,
		const Cell& replacement)
	{
		vector2 d = cell.Direction;

		if (abs(d.major()) < 1) return;

		vector2 l = vector2( d.y, -d.x) + d;
		vector2 r = vector2(-d.y,  d.x) + d;

		bool down = isEmpty(x + int(d.x), y + int(d.y), true);
		bool left = isEmpty(x + int(l.x), y + int(l.y), true);
		bool rght = isEmpty(x + int(r.x), y + int(r.y), true);

		if (down || left || rght) {
			setCell(x, y, _EMPTY);
		}

		if      (down) setCell(x + int(d.x), y + int(d.y), replacement);
		else if (left) setCell(x + int(l.x), y + int(l.y), replacement);
		else if (rght) setCell(x + int(r.x), y + int(r.y), replacement);
	}

	void SandLayer::MoveLikeWater(
		size_t x, size_t y,
		const Cell& cell,
		const Cell& replacement)
	{
		bool down     = isEmpty(x,     y - 1, true);
		bool downleft = isEmpty(x - 1, y - 1, true) && iw::randf() > 0;
		bool downrght = isEmpty(x + 1, y - 1, true) && iw::randf() > 0;
		bool left     = isEmpty(x - 1, y,     true) && iw::randf() > 0;
		bool rght     = isEmpty(x + 1, y,     true) && iw::randf() > 0;

		if (down || downleft || downrght || left || rght) {
			setCell(x, y, _EMPTY);
		}

		if      (down)     setCell(x, y - 1,     replacement);
		else if (downleft) setCell(x - 1, y - 1, replacement);
		else if (downrght) setCell(x + 1, y - 1, replacement);
		else if (left)     setCell(x - 1, y, replacement);
		else if (rght)     setCell(x + 1, y, replacement);
	}

	std::pair<bool, iw::vector2> SandLayer::MoveForward(
		size_t x, size_t y,
		const Cell& cell,
		const Cell& replacement)
	{
		bool hasHit = false;
		iw::vector2 hitLocation;

		std::vector<iw::vector2> cellpos = FillLine(
			x, y, 
			x + cell.Direction.x, y + cell.Direction.y
		);

		for (int i = 0; i < cellpos.size(); i++) {
			float destX = cellpos[i].x;
			float destY = cellpos[i].y;

			bool forward = isEmpty(destX, destY);

			if (cell.Life < 0) {
				setCell(x, y, _EMPTY);
			}

			else if (forward) {
				Cell* c = setCell(destX, destY, replacement);
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

			else if (  !hasHit
					&& inBounds(destX, destY))
			{
				// replace current cell with lower life
				Cell tmp = cell;
				Cell& hit = getCell(destX, destY);
				if (   hit.Type   != cell.Type
					&& hit.TileId != cell.TileId)
				{
					hasHit = true;
					hitLocation = iw::vector2(destX, destY);

					tmp.Life /= 2;
				}

				tmp.Life -= iw::DeltaTime();
				setCell(x, y, tmp);

				if (hasHit) {
					break;
				}
			}
		}

		return { hasHit, hitLocation };
	}

	void SandLayer::HitLikeBullet(
		size_t x, size_t y,
		const Cell& cell)
	{
		Cell& hit = getCell(x, y);

		hit.Life -= cell.Direction.length() * 2;
		if (hit.Life < 0) {
			Cell* c = setCell(x, y, cell);
			if (c) {
				float l = c->Direction.length();

				c->Direction += iw::vector2(iw::randf(), iw::randf()) * l / 2;
				c->Direction.normalize();
				c->Direction *= l;

				c->Life = cell.Life - iw::DeltaTime() / 5;
			}
		}
	}
	
	void SandLayer::HitLikeLaser(
		size_t x, size_t y,
		const Cell& cell)
	{
		Cell& hit = getCell(x, y);

		hit.Life -= cell.Direction.length() * 2;
		if (hit.Life < 0) {
			LOG_INFO << "Hit";

			Cell* c = setCell(x, y, cell);
			if (c) {
				float l = c->Direction.length();

				c->Direction += iw::vector2(iw::randf(), iw::randf()) * l / 4;
				c->Direction.normalize();
				c->Direction *= l;

				c->Life = cell.Life - iw::DeltaTime() / 10;
			}

			//for (size_t i = x - 5; i < x + 5; i++)
			//for (size_t j = y - 5; j < y + 5; j++) {
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

