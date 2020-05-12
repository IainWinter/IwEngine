#pragma once

#include "iw/events/iwevents.h"
#include "iw/log/logger.h"

namespace iw {
namespace events {
	struct event_seq_item {
		virtual bool update() = 0;
	};
	class event_seq {
	private:
		std::vector<event_seq_item*> m_items;
		unsigned m_current;
		bool m_running;

	public:
		IWEVENTS_API
		void add(event_seq_item* item);

		IWEVENTS_API
		void remove(event_seq_item* item);

		IWEVENTS_API
		void update();

		IWEVENTS_API
		void start();

		IWEVENTS_API
		void stop();
	};
}
	using namespace events;
}
