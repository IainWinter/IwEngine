#pragma once

#include "iw/events/iwevents.h"
#include "iw/log/logger.h"

namespace iw {
namespace events {
	struct event_task
	{
		event_task* m_child = nullptr;

		virtual ~event_task()
		{
			delete m_child;
		}

		bool update_tree()
		{
			return update() & (!m_child || m_child->update_tree());
		}

		void reset_tree()
		{
			reset();
			if (m_child) m_child->reset_tree();
		}

		// reset task for another run
		virtual void reset() {}
		// returns true when finished
		virtual bool update() = 0;
	};
}
	using namespace events;
}
