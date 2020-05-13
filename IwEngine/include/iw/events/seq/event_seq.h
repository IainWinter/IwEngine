#pragma once

#include "event_task.h"
#include <vector>

namespace iw {
namespace events {
	class event_seq {
	private:
		std::vector<event_task*> m_tasks;
		unsigned m_current;
		bool m_running;

	public:
		IWEVENTS_API
		event_seq();

		IWEVENTS_API
		~event_seq();

		// Takes ownership of task
		IWEVENTS_API void add   (event_task* task);

		// Only removes doesnt delete. Call this before deleting the sequence to save the task.
		IWEVENTS_API void remove(event_task* task);

		// Resets sequence & tasks
		IWEVENTS_API void reset();

		// Updates current task, moves to next when the current task returns true
		IWEVENTS_API void update();

		// Allows update to run
		IWEVENTS_API void start();

		// Blocks update from running
		IWEVENTS_API void stop();

		// Resets sequence and starts
		IWEVENTS_API void restart();
	};
}
	using namespace events;
}
