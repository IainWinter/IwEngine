#pragma once

#include "iw/engine/Time.h"
#include "iw/math/iwmath.h"

#include <unordered_map>
#include <string>

namespace iw {
namespace Engine {
	struct Timer {
		std::unordered_map<std::string, std::tuple<float, float, float>> Times; // name, current, goal, margin
		size_t CurrentTick = 0;

		Timer() = default;

		void SetTime(
			const std::string& name,
			float time,
			float margin = 0.f)
		{
			Times[name] = std::tuple<float, float, float>(0.f, time, margin);
		}

		// will throw if name isnt a timer
		float& GetTimer (const std::string& name) { return std::get<0>(Times.at(name)); }
		float& GetTime  (const std::string& name) { return std::get<1>(Times.at(name)); }
		float& GetMargin(const std::string& name) { return std::get<2>(Times.at(name)); }

		// will return false if name isnt a timer
		bool Can(
			const std::string& name)
		{
			bool past = PastTime(name);

			if (past) {
				GetTimer(name) = -iw::randf() * GetMargin(name);
			}

			return past;
		}

		// will return false if name isnt a timer
		bool PastTime(
			const std::string& name)
		{
			if (Times.find(name) == Times.end()) {
				return false;
			}

			return GetTime(name) < GetTimer(name);
		}

		void Tick()      { Tick(iw::DeltaTime()); }
		void TickFixed() { Tick(iw::FixedTime()); }

		void Tick(
			float dt)
		{
			CurrentTick++;
			for (auto& p : Times) {
				std::get<0>(p.second) += dt;
			}
		}
	};
}

	using namespace Engine;
}
