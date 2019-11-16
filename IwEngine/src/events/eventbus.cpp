#include "iw/events/eventbus.h"

namespace iw {
	eventbus::eventbus()
		: m_alloc(1024)
	{}

	void eventbus::subscribe(
		const callback<event&> & callback)
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_callbacks.push_back(callback);
	}

	void eventbus::unsubscribe(
		const iw::callback<event&>& callback)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
	}

	void eventbus::publish() {
		std::unique_lock<std::mutex> lock(m_mutex);

		while (!m_events.empty()) {
			iw::event* event = m_events.pop();
			for (auto& callback : m_callbacks) {
				callback(*event);
			}
		}

		m_alloc.reset();
		LOG_INFO << m_alloc.peak();
	}
}
