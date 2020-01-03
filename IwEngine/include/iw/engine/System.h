#pragma once

#include "iw/entity/Space.h"
#include "iw/graphics/Renderer.h"
#include "iw/asset/AssetManager.h"
#include "iw/physics/Dynamics/DynamicsSpace.h"
#include "iw/events/eventbus.h"
#include "Events/Events.h"
#include <queue>
//#include <thread>

namespace IW {
namespace Engine {
	class ISystem {
	public:
		virtual int  Initialize() = 0;
		virtual void Destroy() = 0;
		virtual void Update() = 0;
		virtual void FixedUpdate() = 0;

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

		virtual const char* Name() const = 0;
	};

	template<
		typename... _cs>
	class System
		: public ISystem
	{
	private:
		const char* m_name;
		//std::queue<std::thread>      m_threads;
		std::queue<size_t> m_delete; // Probly make it so space can queue component creation at the ComponentArray level because of templated bs

	protected:
		iw::ref<Space>         Space;
		iw::ref<Renderer>      Renderer;
		iw::ref<AssetManager>  Asset;
		iw::ref<DynamicsSpace> Physics;
		iw::ref<iw::eventbus>  Bus;

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
	public:
		System(
			const char* name)
			: m_name(name)
		{}

		virtual ~System() {}

		virtual int Initialize() {
			return 0;
		}

		virtual void Destroy() {}

		// These wont have to be copies this is just temp

		void Update() override {
			EntityComponentArray eca = Space->Query<_cs...>();
			// Break up view into Viewlets to execute on seperate threads

			// Execute threads
			Update(eca);

			// Execute queues space operations
			while (!m_delete.empty()) {
				size_t index = m_delete.front() ;
				Space->DestroyEntity(index);
				m_delete.pop();
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

		// Action Events

		virtual bool On(ActionEvent& e) override { return false; }

		// Input events

		virtual bool On(MouseWheelEvent&  e) override { return false; }
		virtual bool On(MouseMovedEvent&  e) override { return false; }
		virtual bool On(MouseButtonEvent& e) override { return false; }
		virtual bool On(KeyEvent&         e) override { return false; }
		virtual bool On(KeyTypedEvent&    e) override { return false; }

		// Window events

		virtual bool On(WindowResizedEvent& e) override { return false; }

		// Physics events

		virtual bool On(CollisionEvent& e) override { return false; }

		const char* Name() const override {
			return m_name;
		}
	private:
		friend class Layer;

		void SetLayerVars(
			iw::ref<IW::Space> space,
			iw::ref<IW::Renderer> renderer,
			iw::ref<AssetManager> asset,
			iw::ref<DynamicsSpace> physics,
			iw::ref<iw::eventbus> bus)
		{
			Space = space;
			Renderer = renderer;
			Asset = asset;
			Physics = physics;
			Bus = bus;
		}
	};	
}

	using namespace Engine;
}
