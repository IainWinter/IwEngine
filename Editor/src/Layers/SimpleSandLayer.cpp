#include "Layers/SimpleSandLayer.h"

#include "Sand/SandUpdateSystem.h"

namespace iw {
	SimpleSandLayer::SimpleSandLayer()
		: Layer("Simple Sand")
		, m_world(256, 256, 2) // screen width and height
	{
		srand(time(nullptr));
	}

	int SimpleSandLayer::Initialize() {
		// Setup values for cells

		Cell _EMPTY = {
			CellType::EMPTY,
			CellProperties::NONE,
			iw::Color::From255(0, 0, 0, 0)
		};

		Cell _SAND = {
			CellType::SAND,
			CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE,
			iw::Color::From255(235, 200, 175)
		};

		Cell _WATER = {
			CellType::WATER,
			CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE,
			iw::Color::From255(175, 200, 235)
		};

		Cell _ROCK = {
			CellType::ROCK,
			CellProperties::NONE,
			iw::Color::From255(200, 200, 200)
		};

		Cell::SetDefault(CellType::EMPTY, _EMPTY);
		Cell::SetDefault(CellType::SAND,  _SAND);
		Cell::SetDefault(CellType::WATER, _WATER);
		Cell::SetDefault(CellType::ROCK,  _ROCK);

		Tile ship = {
			{
				              {2,6},
				       {1,5}, {2,5}, 
				{0,4}, {1,4}, {2,4}, {3,4},
				{0,3}, {1,3}, {2,3}, {3,3},
				{0,2},               {3,2},
				{0,1},               {3,1},
				{0,0},               {3,0},
			},
			{
							  _ROCK,
					   _ROCK, _ROCK,
				_ROCK, _ROCK, _ROCK, _ROCK,
				_ROCK, _ROCK, _ROCK, _ROCK,
				_ROCK,               _ROCK,
				_ROCK,               _ROCK,
				_ROCK,               _ROCK,
			}
		};

		shipEntity = Space->CreateEntity<iw::Transform, Tile>();
		shipEntity.Set<iw::Transform>();
		shipEntity.Set<Tile>(ship);

		PushSystem<SandUpdateSystem>(MainScene, m_world, shipEntity.Handle);

		return Layer::Initialize();
	}

	void SimpleSandLayer::PostUpdate() {
		iw::Transform* t = shipEntity.Find<iw::Transform>();

		if (Keyboard::KeyDown(iw::LEFT))  t->Position.x -= 1;
		if (Keyboard::KeyDown(iw::RIGHT)) t->Position.x += 1;
		if (Keyboard::KeyDown(iw::UP))    t->Position.y += 1;
		if (Keyboard::KeyDown(iw::DOWN))  t->Position.y -= 1;
	}

	bool SimpleSandLayer::MoveDown(
		size_t x, size_t y,
		const Cell& cell)
	{
		bool down = m_world.IsEmpty(x, y - 1);
		if (down) {
			//m_world.MoveCell(x, y, x, y - 1);
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
			downLeft  = iw::randf() > 0;
			downRight = !downLeft;
		}

		//if		(downLeft)	m_world.MoveCell(x, y, x - 1, y - 1);
		//else if (downRight) m_world.MoveCell(x, y, x + 1, y - 1);

		return downLeft || downRight;
	}

	bool SimpleSandLayer::MoveSide(
		size_t x, size_t y,
		const Cell& cell)
	{
		bool left  = m_world.IsEmpty(x - 1, y);
		bool right = m_world.IsEmpty(x + 1, y);

		ShuffleIfTrue(left, right);

		//	 if	(left)  m_world.MoveCell(x, y, x - 1, y);
		//else if (right)	m_world.MoveCell(x, y, x + 1, y);

		return left || right;
	}
}

