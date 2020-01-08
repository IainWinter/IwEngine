#pragma once

#include "iwevents.h"
#include "event.h"
#include "callback.h"
#include "iw/util/queue/blocking_queue.h"
#include "iw/util/memory/linear_allocator.h"
#include "iw/log/logger.h"
#include <mutex>

namespace iw {
namespace events {
	class eventbus {
	private:
		std::mutex m_mutex;
		std::vector<callback<event&>> m_callbacks;
		linear_allocator m_alloc;
		blocking_queue<event*> m_events;

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
			_e e(std::forward<_args>(args)...);
			publish_event(&e);
		}
	private:
		IWEVENTS_API
		void publish_event(
			event* e);

		template<
			typename _e>
		_e* alloc_event() {
			event* e = m_alloc.alloc<_e>();
			if (e == nullptr) {
				char* before = m_alloc.memory();
				m_alloc.resize(m_alloc.capacity() * 2);
				char* after = m_alloc.memory();

				int offset = after - before;
				for (event*& eptr : m_events) { // this is such monkey shit but might be the only way cus the types are unknown
					char*& ptr = (char*&)eptr;
					ptr += offset;
				}

				e = m_alloc.alloc<_e>();
			}

			return (_e*)e;
		}
	};
}

	using namespace events;
}
