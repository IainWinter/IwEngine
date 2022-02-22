#pragma once

#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "iw/engine/Components/Timer.h"
#include "Components/EnemyShip.h"
#include "Components/Player.h"
#include "Components/Flocker.h"
#include "Components/Asteroid.h"
#include "Components/CorePixels.h"
#include "Components/Throwable.h"

#include "Events.h"
#include "Levels.h"
#include "Assets.h"
#include "Helpers.h"
#include "ECS.h"

#include <deque>

struct WorldSystem : iw::SystemBase
{
	iw::SandLayer* sand;
	entity m_player;

	iw::Timer m_timer;

	std::deque<iw::EventSequence> m_levels;
	//std::vector<iw::EventSequence> m_runningLevels;

	WorldSystem(
		iw::SandLayer* sand
	)
		: iw::SystemBase("World")
		, sand(sand)
	{
		m_timer.SetTime("asteroid impact", .1f, .25f);
	}

	void Update() override;
	void FixedUpdate() override;

	bool On(iw::ActionEvent& e) override;
	bool On(iw::CollisionEvent& e) override;
private:
	entity MakeAsteroid(SpawnAsteroid_Config& config);

	// put all this in cpp

	Spawn::func_Spawn SpawnEnemy = [=](float x, float y)
	{
		auto [w, h] = sand->GetSandTexSize();
		float margin = .1f;

		float target_x = iw::randi(w - w * margin * 2) + w * margin;
		float target_y = iw::randi(h - h * margin * 2) + h * margin;

		SpawnEnemy_Config config;
		config.SpawnLocationX = x;
		config.SpawnLocationY = y;
		config.TargetLocationX = target_x;
		config.TargetLocationY = target_y;

		std::tie(config.TargetEntity) = entities().query<Player>().only_entity().first();

		config.EnemyType = iw::choose(std::vector<std::pair<EnemyType, float>>{
			{ FIGHTER, 100 },
			{ BOMB,     30 },
			{ STATION,  10 },
			{ BASE,      5 }
		});

		Bus->push<SpawnEnemy_Event>(config);
	};

	Spawn::func_Spawn SpawnAsteroid = [=](float x, float y)
	{
		auto [w, h] = sand->GetSandTexSize();
		float margin = .2f;

		float target_x = iw::randi(w - w * margin * 2) + w * margin;
		float target_y = iw::randi(h - h * margin * 2) + h * margin;

		SpawnAsteroid_Config config;
		config.SpawnLocationX = x;
		config.SpawnLocationY = y;
		config.Size = iw::randi(2);
		config.AngularVel = iw::randfs();

		config.VelocityX = target_x - x;
		config.VelocityY = target_y - y;

		float length = sqrt(config.VelocityX*config.VelocityX + config.VelocityY*config.VelocityY);
		config.VelocityX = config.VelocityX / length * 100;
		config.VelocityY = config.VelocityY / length * 100;

		Bus->push<SpawnAsteroid_Event>(config);
	};

	Spawn MakeEnemySpawner()
	{
		Spawn spawner(iw::randi(10) + 10, iw::randi(3) + 2, iw::randf() + 1.6f, iw::randf() + .3f);
		spawner.OnSpawn = SpawnEnemy;

		AddRandomSides(spawner, 40, 100, 50, 400);

		return spawner;
	}

	Spawn MakeAsteroidSpawner()
	{
		Spawn spawner(iw::randi(5) + 3, iw::randi(2), iw::randf()*2+1, iw::randf());
		spawner.OnSpawn = SpawnAsteroid;

		AddRandomSides(spawner, 100, 200, 200, 500);

		return spawner;
	}

	void AddRandomSides(Spawn& spawner, float minMargin, float maxMargin, float minSize, float maxSize)
	{
		auto [w, h] = sand->GetSandTexSize();
		int s = iw::randi(maxSize - minSize) + minSize;

		for (int i = iw::randi(3) + 1; i > 0; i--)
		{
			int side = iw::randi(3);
			switch (side)
			{
				case 0: spawner.AddSpawn(w / 2 - s, h + minMargin, w / 2 + s, h + maxMargin); break; // top    skinny
				case 1: spawner.AddSpawn(w / 2 - s,   - minMargin, w / 2 + s,   - maxMargin); break; // bottom skinny

				case 3: spawner.AddSpawn(w + minMargin, h / 2 - s, w + maxMargin, h / 2 + s); break; // right skinny
				case 2: spawner.AddSpawn(  - minMargin, h / 2 - s,   - maxMargin, h / 2 + s); break; // left skinny
			}
		}
	}
};
