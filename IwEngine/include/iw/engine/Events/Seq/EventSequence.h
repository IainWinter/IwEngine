#pragma once

#include "iw/engine/Events/Seq/EventTask.h"
#include "iw/events/seq/event_seq.h"

namespace iw {
namespace Engine {
	class Application;
	class Layer;
	class SystemBase;

	class EventSequence
		: event_seq
	{
	private:
		APP_VARS

	public:
		template<
			typename _t,
			typename... _args>
		EventSequence& Add(
			_args&&... args)
		{
			static_assert(std::is_base_of_v<EventTask, _t>, "Type needs to have a base of iw::EventTask");
			return Add(new _t(std::forward<_args>(args)...));
		}

		// takes ownership
		EventSequence& Add(
			EventTask* task)
		{
			task->SetAppVars(MakeAppVars());
			event_seq::add(task);
			return *this;
		}

		// takes ownership
		EventSequence& And(
			EventTask* task)
		{
			task->SetAppVars(MakeAppVars());
			event_seq::and(task);
			return *this;
		}

		template<
			typename _t,
			typename... _args>
		EventSequence& And(
			_args&&... args)
		{
			static_assert(std::is_base_of_v<EventTask, _t>, "Type needs to have a base of iw::EventTask");
			return And(new _t(std::forward<_args>(args)...));
		}

		EventSequence& Add(
			std::function<bool(void)> func)
		{
			add(new event_func(func));
			return *this;
		}

		EventSequence& And(
			std::function<bool(void)> func)
		{
			and(new event_func(func));
			return *this;
		}

		bool Update()                { return update(); }

		void Remove(EventTask* task) { remove(task); }
		void Reset()                 { reset();      }
		void Start()                 { start();      }
		void Stop()                  { stop();       }
		void Restart()               { restart();    }
	private:
		friend class Application;
		friend class Layer;
		friend class SystemBase;

		SET_APP_VARS
		MAKE_APP_VARS
	};
}

	using namespace Engine;
}
