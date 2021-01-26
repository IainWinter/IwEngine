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

//#include "iw/util/memory/pool_allocator.h"

struct Player {
	iw::vector3 Movement = 0; // up, side, boost
	iw::vector3 FireButtons = 0; // bullet, laser (tmp), missiles
	iw::vector2 pMousePos = 0, MousePos = 0; // dont need to be here

	float FireTimeout = 0;

	//int BulletAmmo    = 50; // 50 rounds max, recharge when not firing
	//int MaxBulletAmmo = 50;

	int MissileAmmo    = 46; // 24 missiles max, recharge by mining rez
	int MaxMissileAmmo = 46;

	float Energy = 100;    // 1 second of boost is 10 energy, 1 second of mLASER is 33 energy
	float MaxEnergy = 100;
};

struct Physical {
	iw::vector2 Velocity;

	iw::vector2 Target;
	float Speed = 200;
	float TurnRadius = 0.025f;
	bool HasTarget = false;

	bool AttractTarget = false;

	float Radius = 100;
	float TargetRadius = 200;

	bool Solid = true; // for dust cloud avoidance
};

struct Asteroid { };

struct Flocking {
	bool Active = true;
	//int FlockingWith = 0; // always 0, but then only another id
}; // should have strength of flocking forces

struct EnemyBase;

struct EnemySupplyShip { // attack mode = flee
	int MaxRez = 1500; // 3x return
	int CapturedRez = 0;
};

struct EnemyAttackShip { }; // Tag, attack mode = fight player

struct EnemyShip {
	EnemyBase* Command; // To request commands from
	float ChecekCommandTimer = 0;
	float ChecekCommandTime = 5.0f;

	bool RezLow = false;
	bool AtObjective = false;
	bool AttackMode = false;

	bool PlayerVisible = false;

	bool Homecoming = false;
	bool WantGoHome = false;

	iw::vector2 Objective; // Current location to move twoards (get from Command)
	std::vector<iw::vector2> Objectives; // Random location to move to, same for each group spawned

	//iw::vector2 Velocity  = 0;
	float Speed = 200;

	float TurnRad = 0.025f;
	float FireTimer = 0;
	float FireTime = 0.5f;

	iw::vector2 FireDirection = 0;

	int Rez = 100; // Start with 100 + whatever the cost was to make
	int MinRez = 10;

	int RezToFireLaser = 5;
};

struct EnemyBase {
	EnemyBase* MainBase = nullptr; // Null if main base

	std::vector<iw::EntityHandle> NeedsObjective;
	//std::vector<std::pair<float, iw::vector2>> PlayerLocations; // would be for tracking but that is too hard probz

	iw::vector3 EstPlayerLocation = iw::vector3(0,0, FLT_MAX); // xy + radius

	int Rez = 1000; // Start with ability to spawn 10 ships

	int AttackShipCost = 100;
	int SupplyShipCost = 500;

	float FireTimer = 0;
	float FireTime = 10;

	float UseRezTimer = 0;
	float UseRezTime = 5;

	void RequestObjective(iw::EntityHandle ship) {
		NeedsObjective.push_back(ship);
	}
};

struct Missile {
	float tX = 0;
	float tY = 0;
	float TurnRad = 0.025f;

	float Timer = 0;
	float WaitTime = 0.5f;
	float BurnTime = 3;

	int TileId = 0;
};

#include "Sand/Workers/DefaultSandWorker.h"
#include "Sand/Tile.h"

namespace iw {
	class SpaceSandLayer
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

		iw::ref<iw::Font> m_font;
		Mesh m_textMesh;

		iw::vector2 pMousePos, mousePos;
		float fireTimeout = 0;

		iw::Entity player;

		float spawnEnemy = 0;

		bool reset = false;

		SandWorld world;

		float stepTimer = 0;
		float slowStepTimer = 0;

		int updatedCellCount = 0;

		std::vector<iw::Entity> m_userCleanup; // need to wait one frame to delete shared user data

	public:
		SpaceSandLayer();

		int Initialize() override;
		void PostUpdate() override;

		bool On(MouseMovedEvent& e) override;
		bool On(MouseButtonEvent& e) override;
		bool On(MouseWheelEvent& e) override;
		bool On(KeyEvent& e) override;

		bool On(EntityDestroyedEvent& e) override;
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

			iw::Entity e = Space->CreateEntity<iw::Transform, SharedCellData, Missile, Physical, Flocking>();
			iw::Transform*  t = e.Set<iw::Transform>(point + dir/2);
			SharedCellData* u = e.Set<SharedCellData>();
			Missile*        m = e.Set<Missile>();
			Physical*       p = e.Set<Physical>();
			Flocking*		 f = e.Set<Flocking>();

			m->TileId = whoFiredId;
			m->TurnRad = 0.025 + (iw::randf() + 1) / 25;
			m->WaitTime = 0.5f + iw::randf()/10;
			m->BurnTime = 2 + iw::randf();

			u->UsedForMotion = true;
			u->AssignMotion  = true;
			u->angle = atan2(dir.y, dir.x);
			u->Special = m;
			u->Life = projectile.Life;

			p->Speed = speed;
			p->Velocity = dir * speed;
			p->AttractTarget = true;
			p->TurnRadius = 0.05f;

			f->Active = false;

			Cell cell = projectile;
			cell.TileId = whoFiredId;
			cell.Share = u;
			cell.pX = point.x;
			cell.pY = point.y;
			cell.dX = dir.x * speed;
			cell.dY = dir.y * speed;

			world.SetCell(cell.pX, cell.pY, cell);
		}

		int  UpdateSandWorld  (int fx, int fy, int fx2, int fy2, float deltaTime);
		void UpdateSandTexture(int fx, int fy, int fx2, int fy2);

		void PasteTile(iw::Transform* transform, Tile* tile, bool death = 0, float deltaTime = 0);

		void PasteTiles();
		void RemoveTiles();

		void Reset();

		void _SpawnCluster(float x, float y, float s, int n);
		void GenerateWorld();

		void ExecuteAndWait(
			std::vector<SandChunk*>& chunks,
			std::function<void(SandChunk*)> func)
		{
			std::mutex chunkCountMutex;
			std::condition_variable chunkCountCV;
			int chunkCount = 0;

			for (SandChunk* chunk : chunks) {
				{ std::unique_lock lock(chunkCountMutex); chunkCount++; }
				Task->queue([&, chunk, func]() {
					func(chunk);

					{ std::unique_lock lock(chunkCountMutex); chunkCount--; }
					chunkCountCV.notify_one();
				});
			}

			std::unique_lock lock(chunkCountMutex);
			chunkCountCV.wait(lock, [&]() { return chunkCount == 0; });
		}

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

		// scan a pasted tile
		std::vector<Tile> SplitByFill(
			Tile* tile)
		{
			for (iw::vector2 v : tile->Locations) {
				// translate point or do in local coords, but need a grid for the tile cells because they are in a 1D list without defined bounds

				// if cell isnt filled, fill with id
				// fill cell with new id 
			}

			//std::vector<WorldCoords>

		}

		// https://www.freecodecamp.org/news/flood-fill-algorithm-explained/
		void FloodFill(WorldCoord x, WorldCoord y, std::vector<WorldCoords>& out, const Cell& wall, int targetId) {
			Cell& cell = world.GetCell(x, y);

			if (cell.Filled || cell.Type == wall.Type || cell.TileId != targetId) {
				return;
			}

			cell.Filled = true;
			out.emplace_back(x, y);

			FloodFill(x + 1, y,     out, wall, targetId);
			FloodFill(x - 1, y,     out, wall, targetId);
			FloodFill(x,     y + 1, out, wall, targetId);
			FloodFill(x,     y - 1, out, wall, targetId);
		}
	};
}
