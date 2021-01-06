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

#include "iw/util/set/sparse_set.h"

//#include "iw/util/memory/pool_allocator.h"

struct Player {
	iw::vector3 Movement = 0;
	iw::vector2 pMousePos = 0, MousePos = 0;
	iw::vector2 FireButtons = 0;
	float FireTimeout = 0;

	float Velocity = 0;
};

struct Enemy2 {
	iw::vector2 Spot = 0;
	float FireTimeout = 0;
};

struct Asteroid {
	iw::vector2 Velocity;
};

#include "Sand/Workers/DefaultSandWorker.h"
#include "Sand/Tile.h"

namespace iw {
	class SandLayer
		: public Layer
	{
	private:
		ref<Texture> m_sandTexture; // shorter access than through mesh->material->gettexture
		iw::Mesh m_sandScreen;

		iw::StaticPS m_stars;

		iw::ref<iw::Font> m_font;
		Mesh m_textMesh;

		iw::vector2 pMousePos, mousePos;
		float fireTimeout = 0;

		iw::Entity player;

		float spawnEnemy = 0;

		bool reset = false;

		SandWorld world;

		float stepTimer = 0;

	public:
		SandLayer();

		int Initialize() override;
		void PostUpdate() override;

		bool On(MouseMovedEvent& e) override;
		bool On(MouseButtonEvent& e) override;
		bool On(KeyEvent& e) override;
	private:
		void Fire(
			vector2 position,
			vector2 target,
			float speed,
			const Cell& projectile,
			int whoFiredId)
		{
			iw::vector2 direction = (target - position).normalized();
			iw::vector2 normal = vector2(-direction.y, direction.x);

			iw::vector2 point = position + direction * 25 + normal * iw::randf() * 15;

			//direction = (target - point).normalized();

			if (!world.IsEmpty(point.x, point.y)) {
				return;
			}

			Cell cell = projectile;
			cell.TileId = whoFiredId;
			cell.pX = point.x;
			cell.pY = point.y;
			cell.dX = direction.x * speed;
			cell.dY = direction.y * speed;

			world.SetCell(point.x, point.y, cell);
		}

		void Reset();

		int UpdateSandWorld   (int fx, int fy, int fx2, int fy2);
		void UpdateSandTexture(int fx, int fy, int fx2, int fy2);

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
	};
}
