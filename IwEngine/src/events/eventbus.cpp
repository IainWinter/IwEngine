#include "iw/events/eventbus.h"

namespace iw {
	eventbus::eventbus()
		: m_alloc(1024)
		, m_blocked(false)
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
		std::unique_lock<std::mutex> lock(m_mutex);
		LOG_INFO << "Can't unsub yet lol just make a == operator";
	}

	void eventbus::publish() {
		std::unique_lock<std::mutex> lock(m_mutex);
		if (!m_blocked) {
			while (!m_events.empty()) {
				publish_event(m_events.pop());
			}

			m_alloc.reset();
		}
	}

	void eventbus::pause() {
		LOG_INFO << "Eventbus pasued";
		std::unique_lock<std::mutex> lock(m_mutex);
		m_blocked = true;
	}

	void eventbus::resume() {
		LOG_INFO << "Eventbus resumed!";
		std::unique_lock<std::mutex> lock(m_mutex);
		m_blocked = false;
	}

	void eventbus::publish_event(
		event* e)
	{
		for (auto& callback : m_callbacks) {
			callback(*e);
		}
	}
}
