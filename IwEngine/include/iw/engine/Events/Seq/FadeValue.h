#pragma once

#include "EventTask.h"
#include "iw/entity/Entity.h"
#include "iw/common/Components/Transform.h"
#include "iw/entity/Events/EntityEvents.h"

namespace iw {
namespace Engine {
	template<
		typename _t>
	struct FadeValue
		: EventTask
	{
	private:
		_t* value;
		_t origin;
		_t target;
		float speed;

		bool firstRun;
		float time;

	public:
		FadeValue(
			_t* value,
			_t target,
			float speed = 1.0f)
			: value(value)
			, target(target)
			, speed(speed)
			, firstRun(true)
			, time(0)
		{}

		void reset() override {
			if (!firstRun) {
				*value = origin;
			}

			firstRun = true;
		}

		bool update() override {
			if (!value) return true;

			_t& val = *value;

			if (firstRun) {
				firstRun = false;
				origin = val;
				time = 0;
			}

			time += iw::Time::DeltaTime() * speed;

			val = iw::lerp(origin, target, time);

			if (time >= 1.0f) {
				val = target;
				return true;
			}

			return false;
		}
	};
}

	using namespace Engine;
}
