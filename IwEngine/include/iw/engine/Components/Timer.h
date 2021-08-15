#pragma once

#include "iw/engine/Time.h"
#include "iw/math/iwmath.h"

#include <unordered_map>
#include <string>

namespace iw {
namespace Engine {
	struct Timer {
		std::unordered_map<std::string, std::pair<float, float>> Times; // name, current, goal
		size_t CurrentTick = 0;

		Timer() = default;

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

		// True when timer has past, then resets it to 0
		bool Can(
			const std::string& name)
		{
			bool past = PastTime(name);

			if (past) {
				GetTimer(name) = 0;
			}

			return past;
		}

		// True when timer has past, them resets it to (0, -margin)
		bool Can_random(
			const std::string& name,
			float margin)
		{
			bool past = PastTime(name);

			if (past) {
				GetTimer(name) = -iw::randf() * margin;
			}

			return past;
		}

		bool PastTime(
			const std::string& name)
		{
			return GetTime(name) < GetTimer(name);
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
