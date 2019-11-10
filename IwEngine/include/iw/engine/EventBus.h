#pragma once

#include "Core.h"
#include "iw/events/functional/callback.h"
#include "iw/util/queue/blocking_queue.h"
#include "Events/Event.h"
#include <vector>

namespace IwEngine {
	class IWENGINE_API EventBus {
	private:
		struct {
			std::vector<iwe::callback<Event&>> m_callbacks;
			iwu::blocking_queue<Event*> m_queue;
			std::mutex m_mutex;
		};

	public:
		void subscribe(
			const iwe::callback<Event&>& callback);

		//void unsubscribe(
		//	const iwe::callback<Event&>& callback);

		void push(
			Event* event);

		void publish();
	};
}
