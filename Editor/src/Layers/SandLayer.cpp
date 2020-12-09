#include "Layers/SandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

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

		m_cells.resize(texture->ColorCount() / texture->Channels());
		m_swap .resize(texture->ColorCount() / texture->Channels());

		for (size_t x = 0; x < texture->Width(); x++)
		for (size_t y = 0; y < texture->Height() - 200; y++) {
			m_swap[getCoords(texture, x, y)] = _SAND;
		}

		           player = Space->CreateEntity<iw::Transform, Tile, Player>();
		iw::Entity enemy  = Space->CreateEntity<iw::Transform, Tile, Enemy2>();

		player.Set<iw::Transform>(iw::vector2(texture->Width() / 2, texture->Height() / 2 + 100));
		player.Set<Tile>(Tile {{
						   vector2(1, 0), vector2(1, 0),
			vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
			vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
			vector2(0, 3),                               vector2(3, 3),
			vector2(0, 4),                               vector2(3, 4)
		}});
		player.Set<Player>();

		enemy.Set<iw::Transform>(iw::vector2(texture->Width(), texture->Height()));
		enemy.Set<Tile>(Tile {{
			vector2(1, 0),								 vector2(3, 0),
			vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
			vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
			vector2(0, 3),                               vector2(3, 3),
			vector2(0, 4),                               vector2(3, 4)
		}});

		return 0;
	}

	void SandLayer::PostUpdate() {
		vector2 pos = mousePos / m_scale;
		pos.x = floor(pos.x);
		pos.y = floor(texture->Height() - pos.y);

		if(Keyboard::KeyDown(iw::E) || Keyboard::KeyDown(iw::F) || Keyboard::KeyDown(iw::R)) {
			for (int i = 0; i < 200; i++) {
			for (int ii = 0; ii < 20; ii++) {
				if (inBounds(texture, pos.x + i, pos.y + ii)) {
					size_t index = getCoords(texture, pos.x + i, pos.y + ii);

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
			for (vector2 v : a->Locations) {
				v.x += t->Position.x;
				v.y += t->Position.y;

				setCell(texture, v.x, v.y, _METAL);
			}
		});

		Space->Query<iw::Transform, Tile, Player>().Each([&](
			auto,
			auto t,
			auto,
			auto p)
		{
			p->FireTimeout -= iw::DeltaTime();

			if (   p->FireTimeout < 0
				&& p->FireButtons != 0)
			{
				p->FireTimeout = p->FireButtons.x == 1 ? .1f : .01f;

				vector2 direction = (pos - t->Position).normalized();
				iw::vector2 point = t->Position + direction * 5 + vector2(iw::randf(), iw::randf()) * 5;

				Cell* c = setCell(texture, point.x, point.y, p->FireButtons.x == 1 ? _BULLET : _LASER);
				if(c) c->Direction = direction * (p->FireButtons.x == 1 ? 10 : 25);
			}
		});

		for (size_t x = 0; x < texture->Width();  x++)
		for (size_t y = 0; y < texture->Height(); y++) {
			size_t  i = getCoords(texture, x, y);
			const Cell& cell = m_cells[i];

			switch (cell.Type) {
				case CellType::SAND: {
					MoveLikeSand(x, y, cell, _SAND);
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
			for (vector2 v : a->Locations) {
				v.x += t->Position.x;
				v.y += t->Position.y;

				if (!inBounds(texture, v.x, v.y)) continue;
				Cell& cell = getCell(texture, v.x, v.y);

				if (cell.Type == _METAL.Type) {
					setCell(texture, v.x, v.y, _EMPTY);
				}

				else {
					//v.x = -100000; // `delete`
					//v.y = -100000;
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
			auto,
			auto t,
			auto,
			auto)
		{
			t->Position = iw::lerp<vector2>(
				t->Position,
				iw::vector2(
					texture->Width() / 2 + cos(iw::TotalTime()),
					texture->Height() / 2 + sin(iw::TotalTime())
				),
				iw::DeltaTime());
		});

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
		switch (e.Button) {
			case iw::val(A): player.Find<Player>()->Movement.x -= e.State ? 1 : -1; break;
			case iw::val(D): player.Find<Player>()->Movement.x += e.State ? 1 : -1; break;

			case iw::val(W): player.Find<Player>()->Movement.y += e.State ? 1 : -1; break;
			case iw::val(S): player.Find<Player>()->Movement.y -= e.State ? 1 : -1; break;
		}

		return false;
	}

	void SandLayer::MoveLikeSand(
		size_t x, size_t y,
		const Cell& cell,
		const Cell& replacement)
	{
		bool down = isEmpty(texture, x,     y - 1);
		bool left = isEmpty(texture, x - 1, y - 1);
		bool rght = isEmpty(texture, x + 1, y - 1);

		if (down || left || rght) {
			setCell(texture, x, y, _EMPTY);
		}

		if      (down) setCell(texture, x,     y - 1, replacement);
		else if (left) setCell(texture, x - 1, y - 1, replacement);
		else if (rght) setCell(texture, x + 1, y - 1, replacement);
	}

	void SandLayer::MoveLikeWater(
		size_t x, size_t y,
		const Cell& cell,
		const Cell& replacement)
	{
		bool down     = isEmpty(texture, x, y - 1);
		bool downleft = isEmpty(texture, x - 1, y - 1);
		bool downrght = isEmpty(texture, x + 1, y - 1);
		bool left     = isEmpty(texture, x - 1, y);
		bool rght     = isEmpty(texture, x + 1, y);

		if (down || downleft || downrght || left || rght) {
			setCell(texture, x, y, _EMPTY);
		}

		if      (down)     setCell(texture, x, y - 1,     replacement);
		else if (downleft) setCell(texture, x - 1, y - 1, replacement);
		else if (downrght) setCell(texture, x + 1, y - 1, replacement);
		else if (left)     setCell(texture, x - 1, y, replacement);
		else if (rght)     setCell(texture, x + 1, y, replacement);
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

			bool forward = isEmpty(texture, destX, destY);

			if (cell.Life < 0) {
				setCell(texture, x, y, _EMPTY);
			}

			else if (forward) {
				Cell* c = setCell(texture, destX, destY, replacement);
				if (c) {
					if (i == cellpos.size() - 1) {
						c->Direction = cell.Direction;
					}

					else {
						c->Direction = 0;
					}
				}
			}

			else if (!hasHit && inBounds(texture, destX, destY)) {
				// replace current cell with lower life
				Cell tmp = cell;

				CellType hitType = getCell(texture, destX, destY).Type;

				if (hitType != cell.Type/* && (hitType != CellType::DEBRIS || iw::randf() > .8f)*/) {
					hasHit = true;
					hitLocation = iw::vector2(destX, destY);

					tmp.Life /= 2;
				}

				tmp.Life -= iw::DeltaTime();
				setCell(texture, x, y, tmp);

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
		Cell& hit = m_cells[getCoords(texture, x, y)];

		if (hit.Type != cell.Type) {
			hit.Life -= cell.Direction.length() * 2;
			if (hit.Life < 0) {
				Cell* c = setCell(texture, x, y, cell);
				if (c) {
					float l = c->Direction.length();
					c->Direction += iw::vector2(iw::randf(), iw::randf()) * cell.Direction.length() / 2;

					c->Direction.normalize();
					c->Direction *= l;

					c->Life = cell.Life - iw::DeltaTime() / 5;
				}
			}
		}
	}
	
	void SandLayer::HitLikeLaser(
		size_t x, size_t y,
		const Cell& cell)
	{
		Cell& hit = getCell(texture, x, y);

		if (hit.Type != cell.Type) {
			hit.Life -= cell.Direction.length() * 2;
			if (hit.Life < 0) {
				Cell* c = setCell(texture, x, y, cell);
				if (c) {
					float l = c->Direction.length();
					c->Direction += iw::vector2(iw::randf(), iw::randf()) * cell.Direction.length() / 4;

					c->Direction.normalize();
					c->Direction *= l;

					c->Life = cell.Life - iw::DeltaTime() / 10;
				}

				//for (size_t i = x - 5; i < x + 5; i++)
				//for (size_t j = y - 5; j < y + 5; j++) {
				//	if (   inBounds(texture, i, j)
				//		&& getCell(texture, i, j).Type != cell.Type)
				//	{
				//		if (iw::randf() > .5) {
				//			setCell(texture, i, j, _DEBRIS);
				//		}
				//
				//		else {
				//			setCell(texture, i, j, _EMPTY);
				//		}
				//	}
				//}
			}
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

