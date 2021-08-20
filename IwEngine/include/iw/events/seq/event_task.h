#pragma once

#include "iw/events/iwevents.h"
#include "iw/log/logger.h"

namespace iw {
namespace events {
	struct event_task {
		// reset task for another run
		virtual void reset() {
			LOG_WARNING << "event_task reset without override!";
		}

		// return true when finished
		virtual bool update() = 0;
	};
}
	using namespace events;
}
