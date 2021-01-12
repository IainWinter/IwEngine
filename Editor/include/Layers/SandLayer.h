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

	float Speed = 0;
};

struct Physical {
	iw::vector2 Velocity;

	iw::vector2 Target;
	float Speed = 200;
	float TurnRadius = 0.025f;
	bool HasTarget = false;
};

struct Flocking { }; // Tag, could have strength of flocking forces

struct EnemyBase;

struct EnemySupplyShip {
	float MaxRez = 10000;
};

struct EnemyShip {
	EnemyBase* Command; // To request commands from
	float ChecekCommandTimer = 0;
	float ChecekCommandTime = 5.0f;

	bool RezLow = false;
	bool AtObjective = false;
	bool AttackMode = false;

	int ObjectivesCount = 0;

	iw::vector2 Objective = 0; // Current location to move twoards (get from Command)
	//iw::vector2 Velocity  = 0;
	float Speed = 200;

	float TurnRad = 0.025f;
	float FireTimer = 0;
	float FireTime = 2.5f;

	float Rez = 100; // Start with 100 + whatever the cost was to make
	float MinRez = 10;

	float RezToFireLaser = 5;
};

struct EnemyBase {
	EnemyBase* MainBase = nullptr; // Null if main base

	std::vector<EnemyShip*> NeedsObjective;
	std::vector<std::pair<float, iw::vector2>> PlayerLocations;

	iw::vector3 EstPlayerLocation; // xy + radius

	float Rez = 0;

	float NextGroup = 10;
	float EnemyCost = 5;

	float FireTimer = 0;
	float FireTime = 10;

	void RequestObjective(EnemyShip* ship) {
		NeedsObjective.push_back(ship);
	}

	void LocatedPlayer(iw::vector2 position) {
		PlayerLocations.emplace_back(iw::TotalTime(), position);
	}
};

struct Missile {
	float tX = 0;
	float tY = 0;
	float TurnRad = 0.025f;

	float Timer = 0;
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

		int updatedCellCount = 0;

	public:
		SandLayer();

		int Initialize() override;
		void PostUpdate() override;

		bool On(MouseMovedEvent& e) override;
		bool On(MouseButtonEvent& e) override;
		bool On(KeyEvent& e) override;
	private:
		void Fire(
			iw::Transform* transform,
			vector2 target,
			vector2 vel,
			const Cell& projectile,
			bool atPoint)
		{
			iw::vector2 position = transform->Position;
			int whoFiredId = Space->FindEntity<iw::Transform>(transform).Find<Tile>()->TileId;

			iw::vector2 direction = (target - position).normalized();
			iw::vector2 normal = vector2(-direction.y, direction.x);
			iw::vector2 point = position + direction * 25 + normal * iw::randf() * 15;

			if(atPoint) {
				iw::vector2 d = target - point;
				if (d.length_squared() > 10000) {
					direction = d.normalized(); // for point precision
				}
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
			iw::Transform* transform,
			vector2 target,
			const Cell& projectile)
		{
			iw::vector2 position = transform->Position;
			int whoFiredId = Space->FindEntity<iw::Transform>(transform).Find<Tile>()->TileId;

			iw::vector2 dir = (target - position).normalized();
			iw::vector2 normal = vector2(-dir.y, dir.x);
			iw::vector2 point = position + dir * 25 + normal * iw::randf() * 15;

			// lil random spice
			dir.x += iw::randf()/10;
			dir.y += iw::randf()/10;

			float speed = projectile.Speed();

			iw::Entity e = Space->CreateEntity<SharedCellData, Missile, Physical>();
			SharedCellData* u = e.Set<SharedCellData>();
			Missile*        m = e.Set<Missile>();
			Physical*       p = e.Set<Physical>();

			m->TileId = whoFiredId;
			m->TurnRad = 0.025 + (iw::randf() + 1) / 25;
			m->WaitTime = 0.5f + iw::randf()/10;
			m->BurnTime = 2 + iw::randf();

			u->pX = point.x + dir.x/2;
			u->pY = point.y + dir.y/2;
			u->vX = dir.x * speed;
			u->vY = dir.y * speed;
			u->angle = atan2(dir.y, dir.x);
			u->Special = m;

			p->Speed = speed;

			Cell cell = projectile;
			cell.TileId = whoFiredId;
			cell.User = u;
			cell.pX = point.x;
			cell.pY = point.y;
			cell.dX = dir.x * speed;
			cell.dY = dir.y * speed;

			world.SetCell(cell.pX, cell.pY, cell);
		}

		void Reset();

		int  UpdateSandWorld  (int fx, int fy, int fx2, int fy2, float deltaTime);
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
