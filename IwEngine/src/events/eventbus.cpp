#include "iw/events/eventbus.h"

namespace iw {
	eventbus::eventbus()
		: m_alloc(1024)
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

		int count = 0;
		while (!m_events.empty()) {
			event* e = m_events.pop();
			publish_event(e);
			if (e->Group == -1) {
				LOG_INFO << "Mysterious -1 event";
				continue;
			}

			m_alloc.free(e, e->Size);

			if (count++ > 10000) {
				break;
			}
		}
	}

	void eventbus::publish_event(
		event* e)
	{
		for (auto& callback : m_callbacks) {
			callback(*e);
		}
	}
}
