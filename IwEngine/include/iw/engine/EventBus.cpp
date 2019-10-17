#include "EventBus.h"

namespace IwEngine {
	void EventBus::subscribe(
		const iwe::callback<Event&>& callback)
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		
		m_callbacks.push_back(callback);

		lock.unlock();
	}

	void EventBus::unsubscribe(
		const iwe::callback<Event&>& callback)
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_callbacks.erase(std::find(m_callbacks.begin(), m_callbacks.end(), callback));

		lock.unlock();
	}

	void EventBus::push(
		const Event& event)
	{
		m_queue.push(event);
	}

	void EventBus::publish() {
		std::unique_lock<std::mutex> lock(m_mutex);

		while (!m_queue.empty()) {
			auto& event = m_queue.pop();
			for (auto callback : m_callbacks) {
				callback(event);
			}
		}
	}
}
