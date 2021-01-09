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
	iw::vector3 FireButtons = 0;
	float FireTimeout = 0;

	float Velocity = 0;
};

struct Enemy2 {
	iw::vector2 Spot = 0;
	iw::vector2 Vel = 0;

	float TurnRad = 0.25f;
	float FireTimeout = 0;
};

struct EnemyBase {
	float Energy = 0;

	float NextGroup = 10;
	float EnemyCost = 5;

	float FireTimer = 0;
	float FireTimeout = 4;
	float FireTime = 3;
};

struct Missile {
	float tX = 0;
	float tY = 0;

	float TurnRad = 0.025f;
	float WaitTime = 0.5f;
	float BurnTime = 1;

	int TileId = 0;
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
			vector2 vel,
			const Cell& projectile,
			int whoFiredId,
			bool atPoint)
		{
			iw::vector2 direction = (target - position).normalized();
			iw::vector2 normal = vector2(-direction.y, direction.x);
			iw::vector2 point = position + direction * 25 + normal * iw::randf() * 15;

			if(atPoint) {
				direction = (target - point).normalized(); // for point precision
			}

			//if (!world.IsEmpty(point.x, point.y)) {
			//	return;
			//}

			float speed = projectile.Speed();

			Cell cell = projectile;
			cell.TileId = whoFiredId;
			cell.pX = point.x;
			cell.pY = point.y;
			cell.dX = direction.x * speed + vel.x;
			cell.dY = direction.y * speed + vel.y;

			world.SetCell(point.x, point.y, cell);
		}

		void FireMissile(
			vector2 position,
			vector2 target,
			const Cell& projectile,
			int whoFiredId)
		{
			iw::vector2 dir = (target - position).normalized();
			iw::vector2 normal = vector2(-dir.y, dir.x);
			iw::vector2 point = position + dir * 25 + normal * iw::randf() * 15;

			//direction = (target - point).normalized(); // for point precision

			float speed = projectile.Speed();

			iw::Entity e = Space->CreateEntity<Missile, SharedCellData>();
			Missile*        missile = e.Set<Missile>();
			SharedCellData* user    = e.Set<SharedCellData>();

			missile->TileId = whoFiredId;
			missile->TurnRad = 0.025 + (iw::randf() + 1) / 25;
			missile->WaitTime = 0.5f + iw::randf()/10;
			missile->BurnTime = 2 + iw::randf();

			dir.x += iw::randf()/10;
			dir.y += iw::randf()/10;

			user->pX = point.x + dir.x/2;
			user->pY = point.y + dir.y/2;
			user->vX = dir.x * speed;
			user->vY = dir.y * speed;
			user->Speed = speed;
			user->angle = atan2(dir.y, dir.x);

			user->Special = missile;

			Cell cell = projectile;
			cell.TileId = whoFiredId;
			cell.User = user;
			cell.pX = point.x;
			cell.pY = point.y;
			cell.dX = dir.x * speed;
			cell.dY = dir.y * speed;

			world.SetCell(cell.pX, cell.pY, cell);
		}

		void Reset();

		int  UpdateSandWorld  (int fx, int fy, int fx2, int fy2);
		void UpdateSandTexture(int fx, int fy, int fx2, int fy2);

		void UpdateSharedUserData();
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
