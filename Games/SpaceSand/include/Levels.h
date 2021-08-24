#pragma once

#include "iw/engine/Events/Seq/EventTask.h"
#include "iw/engine/Components/Timer.h"
#include "iw/physics/AABB.h"
#include "glm/vec2.hpp"
#include <vector>
#include <utility>
#include "Events.h"

struct LevelEventTask : iw::EventTask
{
protected:
	iw::Entity Player;
	iw::SandLayer* Sand;
public:
	LevelEventTask(
		iw::Entity m_player,
		iw::SandLayer* sand
	)
		: iw::EventTask()
		, Player(m_player)
		, Sand(sand)
	{}
};

struct Spawn : iw::EventTask
{
	using func_Spawn = std::function<void(float, float)>;

	std::vector<iw::AABB2> Areas;
	int NumberLeftToSpawn;
	int RandBatchSpawn;
	iw::Timer Timer;

	func_Spawn OnSpawn;

	Spawn(
		int numberToSpawn,
		int randBatchSpawn,
		float timeBetweenSpawn,
		float randBetweenSpawn
	)
		: iw::EventTask()
		, NumberLeftToSpawn(numberToSpawn)
		, RandBatchSpawn(randBatchSpawn)
	{
		Timer.SetTime("spawn", timeBetweenSpawn, randBetweenSpawn);
	}

	void AddSpawn(float x, float y, float x2, float y2)
	{
		Areas.emplace_back(glm::vec2(x, y), glm::vec2(x2, y2));
	}

	bool update() override
	{
		Timer.Tick();
		if (Timer.Can("spawn") && Areas.size() > 0)
		{
			for (int i = 0; i < iw::min(iw::randi(RandBatchSpawn) + 1, NumberLeftToSpawn); i++)
			{
				auto [min, max] = Areas.at(iw::randi(Areas.size() - 1));
				float x = min.x + (max.x - min.x) * iw::randf();
				float y = min.y + (max.y - min.y) * iw::randf();

				OnSpawn(x, y);

				NumberLeftToSpawn -= 1;
			}
		}

		return NumberLeftToSpawn == 0;
	}
};

struct Fill : iw::EventTask
{
	enum Direction {
		TOP_BOT,
		LEFT_RIGHT
	};

	std::vector<std::pair<iw::AABB2, Direction>> ToFill; // could add direction to fill
	iw::Timer Timer;

	std::function<void(int, int)> func_Fill;

	Fill(
		std::function<void(int, int)> fill,
		const std::vector<std::pair<iw::AABB2, Direction>>& toFill = {}
	)
		: iw::EventTask()
		, func_Fill(fill)
		, ToFill(toFill)
	{
		Timer.SetTime("drawLine", 0.005f);
	}

	void AddFill(float x, float y, float x2, float y2, Direction direction = TOP_BOT)
	{
		ToFill.emplace_back(iw::AABB2(glm::vec2(x, y), glm::vec2(x2, y2)), direction);
	}

	bool update() override
	{
		Timer.Tick();
		if (Timer.Can("drawLine") && ToFill.size() > 0)
		{
			auto& [min, max] = ToFill.back().first;
			Direction dir    = ToFill.back().second;

			bool done = false;

			switch (dir)
			{
				case TOP_BOT: {
					int y = min.y;
					for (int x = min.x; x < max.x; x++)
					{
						func_Fill(x, y);
					}
					min.y += 1;
					done = min.y >= max.y;

					break;
				}
				case LEFT_RIGHT: {
					int x = min.x;
					for (int y = min.y; y < max.y; y++)
					{
						func_Fill(x, y);
					}
					min.x += 1;
					done = min.x >= max.x;
					break;
				}
			}

			if (done)
			{
				ToFill.pop_back();
			}
		}

		return ToFill.size() == 0;
	}
};
