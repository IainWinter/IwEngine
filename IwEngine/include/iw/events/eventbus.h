#pragma once

#include "iwevents.h"
#include "event.h"
#include "callback.h"
#include "iw/util/queue/blocking_queue.h"
#include "iw/util/memory/pool_allocator.h"
#include "iw/log/logger.h"
#include <mutex>

namespace iw {
namespace events {
	class eventbus {
	private:
		std::mutex m_mutex;
		std::vector<callback<event&>> m_callbacks;
		blocking_queue<event*> m_events;
		pool_allocator m_alloc;

	public:
		IWEVENTS_API
		eventbus();

		IWEVENTS_API
		void subscribe(
			const callback<event&>& callback);

		IWEVENTS_API
		void unsubscribe(
			const callback<event&>& callback);

		IWEVENTS_API
		void publish();

		template<
			typename _e,
			typename... _args>
		void push(
			_args&&... args)
		{
			if (m_events.size() > 500) {
				return;
			}

			_e* e = m_alloc.alloc<_e>();
			if (e != nullptr) {
				new(e) _e(std::forward<_args>(args)...);
				e->Size = sizeof(_e);

				m_events.push(e);
			}
		}

		template<
			typename _e,
			typename... _args>
		void send(
			_args&&... args)
		{
			_e e(std::forward<_args>(args)...);
			e.Size = sizeof(_e);

			publish_event(&e);
		}

		size_t count() {
			return m_events.size();
		}
	private:
		IWEVENTS_API
		void publish_event(
			event* e);
	};
}

	using namespace events;
}
