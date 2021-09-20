#pragma once

#include "iwevents.h"
#include "event.h"
#include "callback.h"
#include "iw/util/queue/blocking_queue.h"
#include "iw/util/memory/pool_allocator.h"
#include "iw/log/logger.h"
#include <mutex>
#include <chrono>
#include <assert.h>

#ifdef IW_USER_REFLECTION
//#	include "iw/_config/engine_reflection.h"
//#	include "iw/_config/user_reflection.h"
#endif

#include "iw/util/reflection/Reflect.h"

namespace iw {
namespace events {
#define TIME std::chrono::system_clock::now().time_since_epoch().count()
	
	struct event_record {
		event* event = nullptr;
		const iw::Type* type = nullptr;

		std::string who_handled;
		std::string who_called;

		size_t time_queued = 0.f;
		size_t time_published = 0.f;
		size_t time_handled = 0.f;

		~event_record()
		{
			delete event;
		}
	};

	struct eventbus_recorder
	{
		std::mutex mutex;
		std::vector<event_record*> records;

		event_record* find_record(size_t id)
		{
			event_record* record = nullptr;
			for (event_record* r : records)
			{
				if (r->event->Id == id)
				{
					record = r;
					break;
				}
			}

			return record;
		}

		template<
			typename _t>
		void record(_t* e, std::string whoCalled)
		{
			event_record* record = new event_record();
			record->event = new _t(*e);
			record->who_called = whoCalled;
			record->time_queued = TIME;
			record->type = iw::GetType<_t>();

			std::unique_lock lock(mutex);

			records.push_back(record);
		}

		void record_published(size_t id)
		{
			std::unique_lock lock(mutex);
			
			event_record* record = find_record(id);
			if (!record)
			{
				LOG_ERROR << "Event doesnt exist";
				return;
			}

			record->time_published = TIME;
		}

		void record_handled(size_t id, std::string whoHandled)
		{
			std::unique_lock lock(mutex);

			event_record* record = find_record(id);
			if (!record)
			{
				LOG_ERROR << "Event doesnt exist";
				return;
			}

			record->who_handled = whoHandled;
			record->time_handled = TIME;
		}
	};

	class eventbus {
	private:
		std::mutex m_mutex;
		std::vector<callback<event&>> m_callbacks;
		blocking_queue<event*> m_events;
		pool_allocator m_alloc;

		eventbus_recorder* m_recorder;

		std::mutex m_idmutex;
		size_t m_next_id;

	public:
		IWEVENTS_API
		eventbus();

		// put in cpp

		void start_record()
		{
			if (m_recorder)
			{
				delete m_recorder;
			}

			m_recorder = new eventbus_recorder();
		}

		std::vector<event_record*> end_record()
		{
			std::vector<event_record*> record = m_recorder->records; // dont delete event*s

			delete m_recorder;
			m_recorder = nullptr;

			return record;
		}

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
			_e* e = m_alloc.alloc<_e>();
			if (e != nullptr) {
				new(e) _e(std::forward<_args>(args)...);
				e->Size = sizeof(_e);

				{
					std::unique_lock lock(m_idmutex);
					e->Id = m_next_id++;
				}

				if (m_recorder)
				{
					m_recorder->record<_e>(e, "");
				}

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

			{
				std::unique_lock lock(m_idmutex);
				e.Id = m_next_id++;
			}

			if (m_recorder)
			{
				m_recorder->record<_e>(&e, "");
			}

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

#undef TIME

//IW_BEGIN_REFLECT
//
//	template<>
//	inline const Class* GetClass(
//		ClassTag<event_record>)
//	{
//		static Class c = Class("event_record", sizeof(event_record), 6);
//
//		//c.fields[0] = ; event pointer, i dont know how to seralize the base class
//		c.fields[1] = { "who_called",    GetClass(ClassTag<std::string>()), offsetof(event_record, who_called)     };
//		c.fields[2] = { "who_handled",   GetClass(ClassTag<std::string>()), offsetof(event_record, who_handled)    };
//		c.fields[3] = { "time_queued",    GetType(TypeTag<float>()),        offsetof(event_record, time_queued)    };
//		c.fields[4] = { "time_published", GetType(TypeTag<float>()),        offsetof(event_record, time_published) };
//		c.fields[5] = { "time_handled",   GetType(TypeTag<float>()),        offsetof(event_record, time_handled)   };
//
//		return &c;
//	}
//	
//IW_END_REFLECT
