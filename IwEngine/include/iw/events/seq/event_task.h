#pragma once

#include "iw/events/iwevents.h"

namespace iw {
namespace events {
	struct event_task {
		// reset task for another run
		virtual void reset() {}

		// return true when finished
		virtual bool update() = 0;
	};
}
	using namespace events;
}
