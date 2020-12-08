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

		for (size_t x = 200; x < texture->Width() - 200; x++)
		for (size_t y = 0; y < texture->Height(); y++) {
			m_swap[getCoords(texture, x, y)] = _SAND;
		}

		return 0;
	}

	void SandLayer::PostUpdate() {
		vector2 pos = mousePos / m_scale;
		pos.x = floor(pos.x);
		pos.y = floor(texture->Height() - pos.y);

		vector2 p = pMousePos;
		for (int i = 0; i < 200; i++) {
			if (inBounds(texture, p.x + i, p.y)) {
				size_t index = getCoords(texture, p.x + i, p.y);

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

		if (Mouse::ButtonDown(iw::LMOUSE) || Mouse::ButtonDown(iw::RMOUSE)) {
			if (iw::randf() > .5f) {
				vector2 direction = (pos - playerTransform.Position).normalized();

				p = playerTransform.Position + direction * 5 + vector2(iw::randf(), iw::randf()) * 5;

				if (inBounds(texture, p.x, p.y)) {
					size_t index = getCoords(texture, p.x, p.y);

					m_swap[index] = Mouse::ButtonDown(iw::LMOUSE) ? _BULLET : _LASER;
					m_swap[index].Direction = direction * (Mouse::ButtonDown(iw::LMOUSE) ? 7 : 15);
				}
			}
		}

		pMousePos = pos;

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
					MoveForward(x, y, cell, _BULLET);
					break;
				}
				case CellType::LASER: {
					MoveForward(x, y, cell, _LASER);
					break;
				}
			}
		}

		//const Cell& swappedCell = m_swap[i];
		//
		//if (   swappedCell.Type != CellType::EMPTY
		//	&& swappedCell.Velocity.length_squared() >= 1)
		//{
		//	size_t dX = x + (size_t)iw::clamp(swappedCell.Velocity.x, -1.f, 1.f);
		//	size_t dY = y + (size_t)iw::clamp(swappedCell.Velocity.y, -1.f, 1.f);
		//
		//	if (   inBounds(texture, dX, dY)
		//		&& isEmpty(texture, dX, dY))
		//	{
		//		size_t vi = getCoords(texture, dX, dY);
		//		m_cells[vi] = swappedCell;
		//		m_cells[i] = _EMPTY;
		//		i = vi;
		//	}
		//}


		std::vector<vector2> playerCoords;
		for (vector2& v : playerCellLocations) {
			//vector4 pos = vector4(v, 5, 1) * playerTransform.WorldTransformation();
			//LOG_INFO << pos;
			playerCoords.push_back(v + playerTransform.Position);
		}

		for (vector2& v : playerCoords) {
			setCell(texture, v.x, v.y, _METAL);
		}

		// Swap buffers
		for (size_t i = 0; i < texture->ColorCount() / texture->Channels(); i++) {
			m_cells[i] = m_swap[i];
			((unsigned int*)texture->Colors())[i] = m_cells[i].Color; // assign RGBA all at once
		}

		// Erase
		for (vector2& v : playerCoords) {
			if (!inBounds(texture, v.x, v.y)) continue;
			m_cells[getCoords(texture, v.x, v.y)] = _EMPTY;
			m_swap [getCoords(texture, v.x, v.y)] = _EMPTY;
		}

		//for (size_t x = 0; x < texture->Width();  x++)
		//for (size_t y = 0; y < texture->Height(); y++) {
		//	if (isColor(texture, x, y, _SAND)) {
		//		bool down = isEmpty(texture, x,     y - 1);
		//		bool left = isEmpty(texture, x - 1, y - 1);
		//		bool rght = isEmpty(texture, x + 1, y - 1);
		//	
		//		if (down || left || rght) {
		//			setColor(texture, x, y, 0);
		//		}

		//		if      (down) setColor(texture, x,     y - 1, _SAND);
		//		else if (left) setColor(texture, x - 1, y - 1, _SAND);
		//		else if (rght) setColor(texture, x + 1, y - 1, _SAND);
		//	}

		//	else if (isColor(texture, x, y, _LASER)) {
		//		bool down = isEmpty(texture, x, y - 1);
		//		unsigned char life = getLifetime(texture, x, y);

		//		if (down) {
		//			setColor(texture, x, y - 1, _LASER); // Expand laser
		//			setLifetime(texture, x, y - 1, life);
		//		}

		//		else if (getColor(texture, x, y - 1) != _LASER) {
		//			setLifetime(texture, x, y,  life/2);

		//			if (getColor(texture, x, y - 1) != _ROCK) {
		//				setColor(texture, x, y - 1, _CLEAR); // Destroy stuff hit by it
		//			}
		//		}

		//		if (life != 0) {
		//			setLifetime(texture, x, y, life * .6);
		//		}

		//		else {
		//			setColor(texture, x, y, _CLEAR);
		//		}
		//	}

		//	//if (x == pos.x && y == pos.y) {
		//	//	setColor(texture, x, y, _SAND);
		//	//}
		//}

		playerTransform.Position +=(playerTransform.Right() * movement.x * 60
								  + playerTransform.Up()    * movement.y * 60)
								  * iw::DeltaTime() * m_scale;

		texture->Update(Renderer->Device); // should be auto in apply filter
		Renderer->ApplyFilter(shader, target, nullptr);
	}

	bool SandLayer::On(
		MouseMovedEvent& e)
	{
		mousePos = { e.X, e.Y };
		return false;
	}

	bool SandLayer::On(
		KeyEvent& e)
	{
		switch (e.Button) {
			case iw::val(A):     movement.x -= e.State ? 1 : -1; break;
			case iw::val(D):     movement.x += e.State ? 1 : -1; break;

			case iw::val(W):     movement.y += e.State ? 1 : -1; break;
			case iw::val(S):     movement.y -= e.State ? 1 : -1; break;
		}

		return false;
	}

	void SandLayer::MoveLikeSand(
		size_t x, size_t y,
		const Cell& cell, const Cell& replacement)
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
		const Cell& cell, const Cell& replacement)
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

	void SandLayer::MoveForward(
		size_t x, size_t y,
		const Cell& cell,
		const Cell& replacement)
	{
		vector2 direction = cell.Direction;
		size_t destX = x + size_t(direction.x);
		size_t destY = y + size_t(direction.y);

		bool forward = isEmpty(texture, destX, destY);

		if (cell.Life < 0) {
			setCell(texture, x, y, _EMPTY);
		}
				
		else if (forward) {
			Cell* c = setCell(texture, destX, destY, replacement);
			if (c) c->Direction = cell.Direction;
		}

		else if (inBounds(texture, destX, destY)) {
			Cell& hit = m_cells[getCoords(texture, destX, destY)];

			Cell tmp = cell;
			tmp.Life -= iw::DeltaTime();

			if (hit.Type != replacement.Type) {
				hit.Life -= cell.Direction.length() * 2;
				if (hit.Life < 0) {
					Cell* c = setCell(texture, destX, destY, replacement);
					if (c) {
						c->Direction = (cell.Direction + iw::vector2(iw::randf(), iw::randf())) * .9f;
						c->Life = cell.Life / 2;
					}

					tmp.Life /= 2;
				}
			}


			setCell(texture, x, y, tmp); // replace current cell with lower life
		}
	}
}

