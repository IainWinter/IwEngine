#pragma once

#include "iw/engine/Time.h"

#include <unordered_map>
#include <string>

namespace iw {
namespace Engine {
	struct Timer {
		std::unordered_map<std::string, std::pair<float, float>> Times; // name, current, goal
		size_t CurrentTick = 0;

		Timer() {}

		void SetTime(
			const std::string& name,
			float time)
		{
			if (Times.find(name) == Times.end()) {
				Times[name].first = 0;
			}

			Times[name].second = time;
		}

		float& GetTime (const std::string& name) { return Times[name].second; }
		float& GetTimer(const std::string& name) { return Times[name].first; }

		bool Can(
			const std::string& name)
		{
			float& timer = GetTimer(name);
			bool   past  = GetTime (name) < timer;

			if (past) {
				timer = 0;
			}

			return past;
		}

		void Tick()      { Tick(iw::DeltaTime()); }
		void TickFixed() { Tick(iw::FixedTime()); }

		void Tick(
			float dt)
		{
			CurrentTick++;
			for (auto& p : Times) {
				p.second.first += dt;
			}
		}
	};
}

	using namespace Engine;
}
