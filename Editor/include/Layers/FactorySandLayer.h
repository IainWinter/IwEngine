#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"
#include <vector>
#include <unordered_map>
#include <mutex>
#include <atomic>

#include <random>

#include "iw/graphics/ParticleSystem.h"
#include "iw/graphics/Font.h"
#include "iw/graphics/ParticleSystem.h"

#include "iw/util/set/sparse_set.h"

struct Player {
	iw::vector2 Movement;
	iw::vector2 Velocity;
	bool OnGround = false;

	bool Fire = false;
	float FireTimeout = 0;
};

#include "Sand/Workers/DefaultSandWorker.h"
#include "Sand/Tile.h"

namespace iw {
	class FactorySandLayer
		: public Layer
	{
	private:
		ref<Texture> m_sandTexture; // shorter access than through mesh->material->gettexture
		iw::Mesh m_sandScreen;

		ref<Texture> m_minimapTexture; // ^^
		iw::Mesh m_minimapScreen;

		iw::Mesh m_teleScreen;
		iw::Mesh m_bulletIconScreen;
		iw::Mesh m_missileIconScreen;
		iw::Mesh m_energyIconScreen;
		iw::Mesh m_energyScreen;
		iw::StaticPS m_bullets;
		iw::StaticPS m_missiles;

		iw::StaticPS m_stars;

		iw::Mesh testMesh;

		iw::ref<iw::Font> m_font;
		Mesh m_textMesh;

		iw::vector2 pMousePos, mousePos;
		float fireTimeout = 0;

		iw::Entity player;

		float spawnEnemy = 0;

		bool reset = false;

		SandWorld world;

		float stepTimer = 0;

		int updatedCellCount = 0;

		std::vector<iw::Entity> m_userCleanup; // need to wait one frame to delete shared user data

		float nextLevelTimer = 0;
		float nextLevelTime = .5;
		float levelTime = 2;
		std::vector<std::tuple<float, float, int>> nextLevels;

	public:
		FactorySandLayer();

		int Initialize() override;
		void PostUpdate() override;

		bool On(MouseMovedEvent& e) override;
		bool On(MouseButtonEvent& e) override;
		bool On(KeyEvent& e) override;

		bool On(EntityDestroyedEvent& e) override;
	private:
		void Fire(
			iw::Transform* transform,
			vector2 target,
			const Cell& projectile,
			int tileId,
			bool atPoint)
		{
			iw::vector2 position = transform->Position;

			iw::vector2 direction = (target - position).normalized();
			iw::vector2 point = position + direction;

			if(atPoint) {
				iw::vector2 d = target - point;
				if (d.length_squared() > 10000) {
					direction = d.normalized(); // for point precision
				}
			}

			if (!world.IsEmpty(point.x, point.y)) {
				return;
			}

			float speed = projectile.Speed();

			Cell cell = projectile;
			cell.pX = point.x;
			cell.pY = point.y;
			cell.dX = direction.x * speed;
			cell.dY = direction.y * speed;
			cell.TileId = tileId;

			world.SetCell(point.x, point.y, cell);
		}

		void Reset();

		void GenerateWorld();

		int  UpdateSandWorld  (int fx, int fy, int fx2, int fy2, float deltaTime);
		void UpdateSandTexture(int fx, int fy, int fx2, int fy2);

		bool PasteTile(iw::Transform* transform, Tile* tile, bool test = false);

		void PasteTiles();
		void RemoveTiles();

		std::pair<int, int> TranslatePoint(vector2 v, vector2 p, float a) {
			float s = sin(a);
			float c = cos(a);

			return {
				ceil(v.x * c - v.y * s + p.x),
				ceil(v.x * s + v.y * c + p.y)
			};
		}

		std::vector<const Cell*> RayCast(
			int x, int y,
			int x2, int y2)
		{
			std::vector<WorldCoords> points = FillLine(x, y, x2, y2);

			std::vector<const Cell*> cells;
			cells.reserve(points.size());

			for (auto [x, y] : points) {
				auto [cx, cy] = world.GetChunkLocation(x, y);
				SandChunk* chunk = world.GetChunkDirect(cx, cy);
				if (!chunk) continue;

				const Cell* cell = &world.GetCell(x, y);
				if (cell->Type != CellType::EMPTY) {
					cells.push_back(&world.GetCell(x, y));
				}
			}

			return cells;
		}

		std::pair<std::pair<bool, bool>, WorldCoords> ShapeCast(
			int x,  int y,
			int x2, int y2,
			iw::Transform* transform,
			Tile* tile)
		{
			WorldCoords max = { x, y };
			bool colX = false,
				colY = false;

			iw::Transform working = *transform;

			// Check the real vector, but if that has a collision not at x2, y2, check each component (x / y) from that max

			std::vector<WorldCoords> points;

			points = FillLine(x, y, x2, y2);
			for (auto [px, py] : points) {
				working.Position = iw::vector2(px, py);

				if (!PasteTile(&working, tile, true)) {
					max = { px, py };
				}

				else {
					colX = colY = true;
					break;
				}
			}

			if (colX /* && colY */) {
				if (max.first != x2) {
					colX = false;
					points = FillLine(x, max.second, x2, max.second);
					for (auto [px, _] : points) {
						working.Position = iw::vector2(px, max.second);

						if (!PasteTile(&working, tile, true)) {
							max.first = px;
						}

						else {
							colX = true;
							break;
						}
					}
				}

				/*else */if (max.second != y2) {
					colY = false;
					points = FillLine(max.first, y, max.first, y2);
					for (auto [_, py] : points) {
						working.Position = iw::vector2(max.first, py);

						if (!PasteTile(&working, tile, true)) {
							max.second = py;
						}

						else {
							colY = true;
							break;
						}
					}
				}
			}

			return {{colX, colY}, max };
		}
	};
}
