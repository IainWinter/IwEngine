#include "iw/events/eventbus.h"

namespace iw {
	eventbus::eventbus()
		: m_alloc(1024, 1)
	{}

	void eventbus::subscribe(
		const callback<event&>& callback)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_callbacks.push_back(callback);
	}

	void eventbus::unsubscribe(
		const callback<event&>& callback)
	{
		//std::unique_lock<std::mutex> lock(m_mutex);
		LOG_INFO << "Can't unsub yet lol just make a == operator";
	}

	void eventbus::publish() {
		std::unique_lock<std::mutex> lock(m_mutex);
		while (!m_events.empty()) {
			publish_event(m_events.pop());
		}

		m_alloc.reset(true);
	}

	void eventbus::publish_event(
		event* e)
	{
		for (auto& callback : m_callbacks) {
			callback(*e);
		}
	}
}
