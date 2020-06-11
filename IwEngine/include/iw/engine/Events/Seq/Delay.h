#pragma once

#include "EventTask.h"
#include "iw/engine/Time.h"

namespace iw {
namespace Engine {
	struct Delay
		: EventTask
	{
	private:
		float timeout;
		float time;

	public:
		Delay(
			float timeout)
			: timeout(timeout)
			, time(0)
		{}

		void reset() override {
			time = 0;
		}

		bool update() override {
			time += iw::Time::DeltaTime();

			if (time > timeout) {
				return true;
			}

			return false;
		}
	};
}

	using namespace Engine;
}
