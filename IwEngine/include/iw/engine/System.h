#pragma once

#include "iw/engine/Events/Seq/EventSequence.h"
#include "iw/engine/Events/Events.h"

// default includes
#include "iw/engine/Time.h"

#include <queue>
#include <string>

namespace iw {
namespace Engine {
	class ISystem {
	public:
		virtual int  Initialize() = 0;
		virtual void Destroy() = 0;
		virtual void Update() = 0;
		virtual void FixedUpdate() = 0;
		virtual void ImGui() = 0;

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

		virtual const std::string& Name() const = 0;
	};

	class SystemBase
		: public ISystem
	{
	private:
		std::string m_name;

	protected:
		APP_VARS

	public:
		SystemBase(
			std::string name)
			: m_name(name)
		{}

		virtual int  Initialize() { return 0;  }
		virtual void Destroy() {}
		virtual void Update() {}
		virtual void FixedUpdate() {}
		virtual void ImGui() {}

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

		const std::string& Name() const override {
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

			if (!a || !b) return true; // no physics components

			bool a1 = a.Has<_t1>();
			bool b1 = b.Has<_t1>();

			if (!a1 && !b1) return true; // no t1

			if (b1) { // b has t1
				iw::Entity t = a;
				a = b;
				b = t;
			}

			if constexpr (std::is_same_v<_t2, void> == false) {
				if (!b.Has<_t2>()) { // no t2
					return true;
				}
			}

			e1 = a;
			e2 = b;

			return false;
		}

		inline CollisionObject* GetPhysicsComponent(
			iw::EntityHandle e)
		{
			CollisionObject* c = Space->FindComponent<CollisionObject>(e);
			if (!c) c = Space->FindComponent<Rigidbody>(e);

			return c;
		}

		inline EventSequence CreateSequence() {
			EventSequence seq;
			seq.SetAppVars(MakeAppVars());

			return seq;
		}
	private:
		friend class Layer;

		SET_APP_VARS
		MAKE_APP_VARS
	};

	template<
		typename... _cs>
	class System
		: public SystemBase
	{
	protected:
		virtual void Update(
			EntityComponentArray& view)
		{}

		virtual void FixedUpdate(
			EntityComponentArray& view)
		{}
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
		}

		void FixedUpdate() override {
			EntityComponentArray eca = Space->Query<_cs...>();
			// Break up view into Viewlets to execute on seperate threads

			// Execute threads
			FixedUpdate(eca);
		}
	};	
}

	using namespace Engine;
}
