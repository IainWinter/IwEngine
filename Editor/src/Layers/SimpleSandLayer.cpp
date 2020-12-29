#include "Layers/SimpleSandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

namespace iw {
	SimpleSandLayer::SimpleSandLayer()
		: Layer("Simple Sand")
		, m_world(1920, 1080, 2) // screen width and height
	{
		srand(time(nullptr));
	}








	int SimpleSandLayer::Initialize() {
		iw::ref<iw::Shader> sandShader = Asset->Load<Shader>("shaders/texture.shader");

		m_sandTexture = REF<iw::Texture>(
			m_world.m_width,
			m_world.m_height,
			iw::TEX_2D,
			iw::RGBA
		);
		m_sandTexture->SetFilter(iw::NEAREST);
		m_sandTexture->CreateColors();
		m_sandTexture->Clear();

		iw::ref<iw::Material> sandScreenMat = REF<iw::Material>(sandShader);
		sandScreenMat->SetTexture("texture", m_sandTexture);

		m_sandMesh = Renderer->ScreenQuad().MakeInstance();
		m_sandMesh.SetMaterial(sandScreenMat);

		// Setup values for cells

		_EMPTY = {
			CellType::EMPTY,
			CellProperties::NONE,
			iw::Color::From255(0, 0, 0, 0)
		};

		_SAND = { 
			CellType::SAND,
			CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE,
			iw::Color::From255(235, 200, 175)
		};

		_WATER = {
			CellType::WATER,
			CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE,
			iw::Color::From255(175, 200, 235)
		};

		_ROCK = {
			CellType::ROCK,
			CellProperties::NONE,
			iw::Color::From255(200, 200, 200)
		};

		Tile t = {
			{
				              {2,0},
				       {1,1}, {2,1}, 
				{0,2}, {1,2}, {2,2}, {3,2},
				{0,3}, {1,3}, {2,3}, {3,3},
				{0,4},               {3,4},
				{0,5},               {3,5},
				{0,6},               {3,6},
			},
			200, 200
		};

		std::vector<std::pair<int, int>> ast;

		float random = 0;

		for (float p = 0; p < Pi2; p += 0.04f) {
			float x = cos(p);
			float y = sin(p);

			random += iw::randf();

			for (float s = 0; s < 100+random; s += 0.04f) {
				if (std::find(ast.begin(), ast.end(), std::make_pair<int, int>(x * s, y * s)) == ast.end()) {
					ast.emplace_back(x * s, y * s);
				}
			}
		}

		Tile t2 = {
			ast,
			200, 200
		};

		m_tiles.push_back(t2);

		return 0;
	}

	void SimpleSandLayer::PostUpdate() {
		vector2 pos = Mouse::ScreenPos() / m_world.m_scale;
		pos.y = m_world.m_height - pos.y;

		if (Mouse::ButtonDown(iw::LMOUSE)) {
			Cell placeMe = _EMPTY;

				 if (Keyboard::KeyDown(iw::S)) placeMe = _SAND;
			else if (Keyboard::KeyDown(iw::W)) placeMe = _WATER;
			else if (Keyboard::KeyDown(iw::R)) placeMe = _ROCK;

			for (size_t x = 0; x < 20; x++)
			for (size_t y = 0; y < 20; y++) {
				if (!m_world.InBounds(pos.x + x, pos.y + y)) continue;
				
				m_world.SetCell(x, y, placeMe);
			}
		}

		if (Keyboard::KeyDown(iw::LEFT))  m_tiles[0].X -= 1;
		if (Keyboard::KeyDown(iw::RIGHT)) m_tiles[0].X += 1;
		if (Keyboard::KeyDown(iw::UP))    m_tiles[0].Y += 1;
		if (Keyboard::KeyDown(iw::DOWN))  m_tiles[0].Y -= 1;

		for (Tile& tile : m_tiles) {
			for (auto [x, y] : tile.Positions) {
				x += tile.X;
				y += tile.Y;

				if (m_world.InBounds(x, y)) {
					// what happens if the cell is already full?
					m_world.SetCell(x, y, _ROCK);
				}
			}
		}

		// Update cells

		for (size_t x = 0; x < m_world.m_width;  x++)
		for (size_t y = 0; y < m_world.m_height; y++) {
			const Cell& cell = m_world.GetCell(x, y);

				 if (cell.Props & CellProperties::MOVE_DOWN		 && MoveDown	(x, y, cell)) {}
			else if (cell.Props & CellProperties::MOVE_DOWN_SIDE && MoveDownSide(x, y, cell)) {}
			else if (cell.Props & CellProperties::MOVE_SIDE		 && MoveSide	(x, y, cell)) {}
		}

		m_world.CommitCells();
		
		// Update the sand texture

		unsigned int* pixels = (unsigned int*)m_sandTexture->Colors();
		for (size_t i = 0; i < m_world.m_width * m_world.m_height; i++) {
			pixels[i] = m_world.GetCell(i).Color;
		}

		if (m_world.InBounds(pos.x, pos.y)) {
			pixels[m_world.GetIndex(pos.x, pos.y)] = iw::Color(1, 0, 0);
		}
		
		for (Tile& tile : m_tiles) {
			for (auto [x, y] : tile.Positions) {
				x += tile.X;
				y += tile.Y;

				if (m_world.InBounds(x, y)) {
					// what happens if the cell is no longer part of the tile?
					m_world.SetCell(x, y, _EMPTY);
				}
			}
		}

		// Draw the sand to the screen

		m_sandTexture->Update(Renderer->Device);

		Renderer->BeginScene(MainScene);
		Renderer->	DrawMesh(iw::Transform(), m_sandMesh);
		Renderer->EndScene();
	}
	
	bool SimpleSandLayer::MoveDown(
		size_t x, size_t y,
		const Cell& cell)
	{
		bool down = m_world.IsEmpty(x, y - 1);
		if (down) {
			m_world.MoveCell(x, y, x, y - 1);
		}

		return down;
	}
	
	bool SimpleSandLayer::MoveDownSide(
		size_t x, size_t y,
		const Cell& cell)
	{
		bool downLeft  = m_world.IsEmpty(x - 1, y - 1);
		bool downRight = m_world.IsEmpty(x + 1, y - 1);

		if (downLeft && downRight) {
			bool rand = iw::randf() > 0;
			downLeft = rand ? true : false;
			downRight = rand ? false : true;
		}

		if		(downLeft)	m_world.MoveCell(x, y, x - 1, y - 1);
		else if (downRight) m_world.MoveCell(x, y, x + 1, y - 1);

		return downLeft || downRight;
	}

	bool SimpleSandLayer::MoveSide(
		size_t x, size_t y,
		const Cell& cell)
	{
		bool left  = m_world.IsEmpty(x - 1, y);
		bool right = m_world.IsEmpty(x + 1, y);

		ShuffleIfTrue(left, right);

			 if	(left)  m_world.MoveCell(x, y, x - 1, y);
		else if (right)	m_world.MoveCell(x, y, x + 1, y);

		return left || right;
	}
}

