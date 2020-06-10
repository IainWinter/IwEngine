#pragma once

#include "EventTask.h"
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
		ref<Space>          Space;
		ref<QueuedRenderer> Renderer;
		ref<AssetManager>   Asset;
		ref<DynamicsSpace>  Physics;
		ref<AudioSpace>     Audio;
		ref<eventbus>       Bus;

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
			task->SetVars(Space, Renderer, Asset, Physics, Audio, Bus);
			add(task);
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

		void SetVars(
			ref<iw::Space>      space,
			ref<QueuedRenderer> renderer,
			ref<AssetManager>   asset,
			ref<DynamicsSpace>  physics,
			ref<AudioSpace>     audio,
			ref<eventbus>       bus)
		{
			Space = space;
			Renderer = renderer;
			Asset = asset;
			Physics = physics;
			Audio = audio;
			Bus = bus;
		}
	};
}

	using namespace Engine;
}
