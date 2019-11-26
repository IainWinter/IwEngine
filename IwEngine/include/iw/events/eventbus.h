#pragma once

#include "iwevents.h"
#include "event.h"
#include "callback.h"
#include "iw/util/queue/blocking_queue.h"
#include "iw/util/memory/linear_allocator.h"
#include "iw/log/logger.h"
#include <mutex>

namespace iw {
inline namespace events {
	class eventbus {
	private:
		std::vector<iw::callback<event&>> m_callbacks;
		iw::linear_allocator m_alloc;
		iw::blocking_queue<event*> m_events;
		std::mutex m_mutex;

	public:
		IWEVENTS_API eventbus();

		IWEVENTS_API void subscribe(
			const iw::callback<event&>& callback);

		IWEVENTS_API void unsubscribe(
			const iw::callback<event&>& callback);

		IWEVENTS_API void publish();

		template<
			typename _e = event>
		void push(
			const event& e)
		{
			iw::event& event = allocAndPushEvent<_e>();
			event = (const _e&)e;
		}

		template<
			typename _e,
			typename... _args>
		void push(
			_args&&... args)
		{
			iw::event& event = allocAndPushEvent<_e>();
			event = _e{ std::forward<_args>(args)... };
		}
	private:
		template<
			typename _e>
		event& allocAndPushEvent() {
			event* e = m_alloc.alloc<_e>();
			if (e == nullptr) {
				m_alloc.resize(m_alloc.capacity() * 2);
				e = m_alloc.alloc<_e>();
			}

			m_events.push(e);

			return *e;
		}
	};
}
}
