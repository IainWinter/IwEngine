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
			typename _e,
			typename... _args>
		void push(
			_args&&... args)
		{
			_e* e = alloc_event<_e>();
			*e = _e{ std::forward<_args>(args)... };
			m_events.push(e);
		}

		template<
			typename _e,
			typename... _args>
		void send(
			_args&&... args)
		{
			_e* e = alloc_event<_e>();
			*e = _e{ std::forward<_args>(args)... };
			publish_event(e);
		}
	private:
		template<
			typename _e>
		_e* alloc_event() {
			event* e = m_alloc.alloc<_e>();
			if (e == nullptr) {
				m_alloc.resize(m_alloc.capacity() * 2);
				e = m_alloc.alloc<_e>();
			}

			return (_e*)e;
		}

		void publish_event(
			event* e);
	};
}
}
