#pragma once

#include "Core.h"
#include "iw/events/functional/callback.h"
#include "iw/util/queue/blocking_queue.h"
#include "Events/Event.h"
#include <vector>

namespace IwEngine {
	class EventBus {
	private:
		std::vector<iwe::callback<Event&>> m_callbacks;
		iwu::blocking_queue<Event*> m_queue;
		std::mutex m_mutex;

	public:
		IWENGINE_API void subscribe(
			const iwe::callback<Event&>& callback);

		//void unsubscribe(
		//	const iwe::callback<Event&>& callback);

		IWENGINE_API void push(
			Event* event);

		IWENGINE_API void publish();
	};
}
