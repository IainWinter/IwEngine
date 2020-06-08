#pragma once

#include "Events/Events.h"
#include "iw/entity/Space.h"
#include "iw/graphics/QueuedRenderer.h"
#include "iw/asset/AssetManager.h"
#include "iw/physics/Dynamics/DynamicsSpace.h"
#include "iw/audio/AudioSpace.h"
#include "iw/events/eventbus.h"
#include <queue>
//#include <thread>

namespace iw {
namespace Engine {
	class ISystem {
	public:
		virtual int  Initialize() = 0;
		virtual void Destroy() = 0;
		virtual void Update() = 0;
		virtual void FixedUpdate() = 0;

		virtual void OnPush() = 0;
		virtual void OnPop() = 0;

		// Action events

		virtual bool On(ActionEvent& e) = 0;

		// Input events

		virtual bool On(MouseWheelEvent& e) = 0;
		virtual bool On(MouseMovedEvent& e) = 0;
		virtual bool On(MouseButtonEvent& e) = 0;
		virtual bool On(KeyEvent& e) = 0;
		virtual bool On(KeyTypedEvent& e) = 0;

		// Window events

		virtual bool On(WindowResizedEvent& e) = 0;

		// Physics events

		virtual bool On(CollisionEvent& e) = 0;

		// Entity events

		virtual bool On(EntityDestroyEvent& e) = 0;
		virtual bool On(EntityDestroyedEvent& e) = 0;

		virtual const char* Name() const = 0;
	};

	class SystemBase
		: public ISystem
	{
	private:
		const char* m_name;

	protected:
		ref<Space>               Space;
		ref<QueuedRenderer>      Renderer;
		ref<AssetManager>        Asset;
		ref<DynamicsSpace>       Physics;
		ref<AudioSpace>          Audio;
		ref<eventbus>            Bus;

	public:
		SystemBase(
			const char* name)
			: m_name(name)
		{}

		virtual int  Initialize() { return 0;  }
		virtual void Destroy() {}
		virtual void Update() {}
		virtual void FixedUpdate() {}

		virtual void OnPush() override {}
		virtual void OnPop() override {}

		// Action Events

		virtual bool On(ActionEvent& e) override { return false; }

		// Input events

		virtual bool On(MouseWheelEvent& e) override { return false; }
		virtual bool On(MouseMovedEvent& e) override { return false; }
		virtual bool On(MouseButtonEvent& e) override { return false; }
		virtual bool On(KeyEvent& e) override { return false; }
		virtual bool On(KeyTypedEvent& e) override { return false; }

		// Window events

		virtual bool On(WindowResizedEvent& e) override { return false; }

		// Physics events

		virtual bool On(CollisionEvent& e) override { return false; }

		// Entity events

		virtual bool On(EntityDestroyEvent& e) override { return false; }
		virtual bool On(EntityDestroyedEvent& e) override { return false; }

		const char* Name() const override {
			return m_name;
		}
	protected:
		// Tries to find entities with CollisionObject / Rigidbody components.
		// Returns true if there were no entities.
		// if _t2 is specified, checks if that component exists on entity 2
		template<
			typename _t1,
			typename _t2 = void>
		bool GetEntitiesFromManifold(
			const Manifold& manifold,
			iw::Entity& e1,
			iw::Entity& e2)
		{
			iw::Entity a = Space->FindEntity(manifold.ObjA);
			iw::Entity b = Space->FindEntity(manifold.ObjB);

			if (!a) a = Space->FindEntity<iw::Rigidbody>(manifold.ObjA);
			if (!b) b = Space->FindEntity<iw::Rigidbody>(manifold.ObjB);

			if (!a || !b) return true;

			if (   !a.Has<_t1>()
				&& b.Has<_t1>())
			{
				iw::Entity t = a;
				a = b;
				b = t;
			}

			else {
				return true;
			}

			if constexpr (std::is_same_v<_t2, void> == false) {
				if (!b.Has<_t2>()) {
					return true;
				}
			}

			e1 = a;
			e2 = b;

			return false;
		}
	private:
		friend class Layer;

		void SetLayerVars(
			iw::ref<iw::Space> space,
			iw::ref<iw::QueuedRenderer> renderer,
			iw::ref<AssetManager> asset,
			iw::ref<DynamicsSpace> physics,
			iw::ref<AudioSpace> audio,
			iw::ref<iw::eventbus> bus)
		{
			Space = space;
			Renderer = renderer;
			Asset = asset;
			Physics = physics;
			Audio = audio;
			Bus = bus;
		}
	};

	template<
		typename... _cs>
	class System
		: public SystemBase
	{
		struct PropChange { // only supports integral types
			void* prop;
			void* value;
			size_t size;
		};

	private:
		//std::queue<std::thread>      m_threads;
		std::queue<size_t> m_delete; // Probly make it so space can queue component creation at the ComponentArray level because of templated bs
		std::queue<PropChange> m_changes;

	protected:
		virtual void Update(
			EntityComponentArray& view)
		{}

		virtual void FixedUpdate(
			EntityComponentArray& view)
		{}

		void QueueDestroyEntity(
			size_t index)
		{
			m_delete.push(index);
		}

		template<
			typename _t>
		void QueueChange(
			_t* prop,
			_t  value)
		{
			static_assert(std::is_integral_v<_t>); // make sure isnt like a vector or something

			m_changes.push({
				prop,
				new (_t)(value),
				sizeof(_t)
			});
		}
	public:
		System(
			const char* name)
			: SystemBase(name)
		{}

		virtual int Initialize() {
			return 0;
		}

		// These wont have to be copies this is just temp

		void Update() override {
			EntityComponentArray eca = Space->Query<_cs...>();
			// Break up view into Viewlets to execute on seperate threads

			// Execute threads
			Update(eca);

			while (!m_delete.empty()) {
				size_t index = m_delete.front();
				Space->DestroyEntity(index);
				m_delete.pop();
			}

			while (!m_changes.empty()) {
				PropChange change = m_changes.front();

				memcpy(change.prop, change.value, change.size);
				delete change.value;

				m_changes.pop();
			}
		}

		void FixedUpdate() override {
			EntityComponentArray eca = Space->Query<_cs...>();
			// Break up view into Viewlets to execute on seperate threads

			// Execute threads
			FixedUpdate(eca);

			// Execute queues space operations
			while (!m_delete.empty()) {
				size_t index = m_delete.front();
				Space->DestroyEntity(index);
				m_delete.pop();
			}
		}
	};	
}

	using namespace Engine;
}
