#pragma once

#include "event_task.h"
#include <vector>
#include <functional>

namespace iw {
namespace events {
	struct event_func
		: event_task
	{
		std::function<bool(void)> func;

		bool update() override {
			return func();
		}
	};

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

		// break design pattern if it looks better xd

		// Takes ownership of task
		IWEVENTS_API
		void add(
			event_task* task);

		IWEVENTS_API
		event_func* add(
			std::function<bool(void)> func);

		// Only removes doesnt delete.
		// Call this before deleting the sequence to save the task.
		IWEVENTS_API
		void remove(
			event_task* task);

		// Deletes all events, remove them before clearing to save them. 
		IWEVENTS_API
		void clear();

		// Resets sequence & tasks
		IWEVENTS_API
		void reset();

		// Updates current task, moves to next when the current task returns true
		IWEVENTS_API
		void update();

		// Allows update to run
		IWEVENTS_API
		void start();

		// Blocks update from running
		IWEVENTS_API
		void stop();

		// Resets sequence and starts
		IWEVENTS_API
		void restart();
	};
}
	using namespace events;
}
