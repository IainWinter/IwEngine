#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include <vector>

enum class CellType {
	EMPTY,

	SAND,
	WATER,
	ROCK,
	METAL,
	DEBRIS,

	LASER,
	BULLET
};

struct Cell {
	CellType Type;
	iw::Color Color;

	float Life = 0;
	iw::vector2 Direction = -iw::vector2::unit_y;

	bool Gravitised = false;

	size_t TileId = 0; // 0 = not part of tile
};

struct Tile {
	std::vector<iw::vector2> Locations;
	const size_t InitialLocationsSize = 0;
	size_t TileId;

	Tile() = default;

	Tile(std::vector<iw::vector2> locations)
		: InitialLocationsSize(locations.size() * 4)
	{
		for (iw::vector2& v : locations) {
			Locations.push_back(v * 2 + iw::vector2(1, 0) - 4);
			Locations.push_back(v * 2 + iw::vector2(1, 1) - 4);
			Locations.push_back(v * 2 + iw::vector2(0, 1) - 4);
			Locations.push_back(v * 2 + iw::vector2(0, 0) - 4);
		}

		static size_t s_tileId = 1;
		TileId = s_tileId++;
	}
};

struct Player {
	iw::vector3 Movement = 0;
	iw::vector2 pMousePos = 0, MousePos = 0;
	iw::vector2 FireButtons = 0;
	float FireTimeout = 0;
};

struct Enemy2 {
	iw::vector2 Spot = 0;
	float FireTimeout = 0;
};

namespace iw {
	class SandLayer
		: public Layer
	{
	private:
		ref<Shader> shader;

		ref<Texture> texture;
		ref<RenderTarget> target;

		float m_scale;

		std::vector<Cell> m_cells;
		std::vector<Cell> m_swap;

		const Cell _EMPTY  = { CellType::EMPTY,  Color(0), 0, 0 };

		const Cell _SAND   = { CellType::SAND,   Color::From255(237, 201, 175), 1 };
		const Cell _WATER  = { CellType::WATER,  Color::From255(175, 201, 237)};
		const Cell _ROCK   = { CellType::ROCK,   Color::From255(201, 201, 201)};
		const Cell _METAL  = { CellType::METAL,  Color::From255(230, 230, 230), 10 };
		const Cell _DEBRIS = { CellType::DEBRIS, Color::From255(150, 150, 150), 1 };

		const Cell _LASER  = { CellType::LASER,  Color::From255(255,   0,   0), .06f };
		const Cell _BULLET = { CellType::BULLET, Color::From255(255,   255, 0), .02f };

		iw::vector2 pMousePos, mousePos;
		float fireTimeout = 0;

		iw::Entity player;

		iw::vector2 gravPos;

		float spawnEnemy = 0;

		bool reset = false;

	public:
		SandLayer();

		int Initialize() override;
		void PostUpdate() override;

		bool On(MouseMovedEvent& e) override;
		bool On(MouseButtonEvent& e) override;
		bool On(KeyEvent& e) override;
	private:
		void MoveLikeSand(
			size_t x, size_t y,
			Cell& cell,
			const Cell& replacement);

		void MoveLikeWater(
			size_t x, size_t y,
			const Cell& cell,
			const Cell& replacement);

		std::pair<bool, iw::vector2> MoveForward(
			size_t x, size_t y,
			const Cell& cell,
			const Cell& replacement);

		void HitLikeBullet(
			size_t x, size_t y,
			const Cell& replacement);

		void HitLikeLaser(
			size_t x, size_t y,
			const Cell& replacement);

		void Fire(
			vector2 position,
			vector2 target,
			float speed,
			const Cell& cell,
			size_t whoFiredId)
		{
			vector2 direction = (target - position).normalized();
			iw::vector2 point = position + direction * iw::vector2(5, 7) + vector2(iw::randf(), iw::randf()) * 5;

			if (    inBounds(point.x, point.y)
				&& getCell(point.x, point.y).Type != CellType::EMPTY)
			{
				return;
			}

			Cell* c = setCell(point.x, point.y, cell);
			if (c) {
				c->Direction = direction * speed;
				c->TileId = whoFiredId;
			}
		}

		std::vector<iw::vector2> FillLine(
			int x,  int y,
			int x2, int y2);

		size_t getCoords(
			size_t x, size_t y)
		{
			return x + y * texture->Width();
		}

		bool inBounds(
			size_t x, size_t y)
		{
			return  x < texture->Width()
				&& y < texture->Height();
		}

		bool isEmpty(
			size_t x, size_t y,
			bool bothBuffers = false)
		{
			if (!inBounds(x, y)) return false;
			if (bothBuffers) {
				return  m_cells[getCoords(x, y)].Type == _EMPTY.Type
					&& m_swap[getCoords(x, y)].Type == _EMPTY.Type;
			}

			return m_cells[getCoords(x, y)].Type == _EMPTY.Type;
		}

		Cell* setCell(
			size_t x, size_t y,
			const Cell& cell,
			bool bothBuffers = false)
		{
			if (inBounds(x, y)) {
				size_t index = getCoords(x, y);

				if (bothBuffers) {
					m_cells[index] = cell;
				}

				return &(m_swap[index] = cell);
			}

			return nullptr;
		}

		Cell& getCell(
			size_t x, size_t y,
			bool getFromSwap = false)
		{
			return getFromSwap ? m_swap[getCoords(x, y)] : m_cells[getCoords(x, y)];
		}
	};
}
