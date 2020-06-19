#pragma once

#include "EventTask.h"
#include "iw/engine/AppVars.h"
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
		EventTask* Add(
			_args&&... args)
		{
			static_assert(std::is_base_of_v<EventTask, _t>, "Type needs to have a base of iw::EventTask");

			EventTask* task = new _t(std::forward<_args>(args)...);
			Add(task);

			return task;
		}

		void Add(
			EventTask* task)
		{
			task->SetAppVars(MakeAppVars());
			add(task);
		}

		event_func* Add(
			std::function<bool(void)> func)
		{
			return add(func);
		}

		void Remove(
			EventTask* task)
		{
			remove(task);
		}

		void Reset() {
			reset();
		}

		void Update() {
			update();
		}

		void Start() {
			start();
		}

		void Stop() {
			stop();
		}

		void Restart() {
			restart();
		}
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
