#pragma once

#include "iw/engine/System.h"
#include "iw/engine/EventStack.h"

// default includes
#include "iw/graphics/Scene.h"

#undef PlaySound

namespace iw {
namespace Engine {
	class Application;

	class Layer {
	private:
		std::string m_name;
		EventStack<ISystem*> m_systems; // layer doesnt own systems but prolly should
	protected:
		APP_VARS

		Scene* MainScene;

	public:
		bool IsInitialized;

		Layer(
			std::string name
		)
			: m_name(name)
			, MainScene(new Scene())
			, IsInitialized(false)
		{}

		~Layer() {
			delete MainScene;
		}

		virtual int Initialize()
		{
			for (ISystem* s : m_systems) {
				int e = s->Initialize();
				if (e != 0) return e;
			}

			IsInitialized = true;

			return 0;
		}

		virtual void Destroy()
		{
			for (ISystem* s : m_systems) {
				s->Destroy();
			}
		}

		// Sync Updates

		virtual void ImGui()
		{
			for (ISystem* s : m_systems) {
				s->ImGui();
			}
		}

		virtual void PreUpdate() {}
		virtual void Update() {} // this one should be in thread pool btw 'AsyncUpdate' is maybe a better name
		virtual void PostUpdate() {}

		virtual void FixedUpdate() {}

		virtual void OnPush() {}
		virtual void OnPop() {}

		// Action events

		virtual bool On(ActionEvent& e) { return m_systems.On(e); }

		// Input events

		virtual bool On(MouseWheelEvent& e)  { return m_systems.On(e); }
		virtual bool On(MouseMovedEvent& e)  { return m_systems.On(e); }
		virtual bool On(MouseButtonEvent& e) { return m_systems.On(e); }
		virtual bool On(KeyEvent& e)         { return m_systems.On(e); }
		virtual bool On(KeyTypedEvent& e)    { return m_systems.On(e); }

		// Window events

		virtual bool On(WindowResizedEvent& e) { return m_systems.On(e); }

		// Os events

		virtual bool On(OsEvent& e) { return m_systems.On(e); }

		// Physics events

		virtual bool On(CollisionEvent& e) { return m_systems.On(e); }

		// Entity events

		virtual bool On(EntityCreatedEvent& e)   { return m_systems.On(e); }
		virtual bool On(EntityDestroyEvent& e)   { return m_systems.On(e); }
		virtual bool On(EntityDestroyedEvent& e) { return m_systems.On(e); }
		virtual bool On(EntityMovedEvent& e)     { return m_systems.On(e); }

		// System updates

		virtual void UpdateSystems()
		{
			for (ISystem* system : m_systems) {
				LOG_TIME_SCOPE(system->Name() + " system");
				system->Update();
			}
		}

		virtual void FixedUpdateSystems()
		{
			for (ISystem* system : m_systems) {
				system->FixedUpdate();
			}
		}

		const std::string& Name() const {
			return m_name;
		}

		EventStack<ISystem*>& temp_GetSystems() {
			return m_systems;
		}

		template<
			typename S>
		S* GetSystem(
			const char* name)
		{
			return (S*)m_systems.Get(name);
		}

		// should add a function for making a system that
		// assigneds app vars but doesnt push it to the layer

		template<
			typename S,
			typename... Args>
		S* PushSystem(
			Args&&... args)
		{
			S* system = new S(std::forward<Args>(args)...);
			system->SetAppVars(MakeAppVars());

			PushSystem(system);
			return system;
		}

		template<
			typename S>
		void PushSystem(
			S* system)
		{
			std::string sname = system->Name();
			std::string lname = Name();

			LOG_INFO << "Pushed " << sname << " system onto " << lname << " layer";
			system->SetAppVars(MakeAppVars());
			m_systems.PushBack(system);

			system->OnPush();
		}

		template<
			typename S,
			typename... Args>
		S* PushSystemFront(
			Args&&... args)
		{
			S* system = new S(std::forward<Args>(args)...);
			system->SetAppVars(MakeAppVars());

			PushSystemFront(system);
			return system;
		}

		template<
			typename S>
		void PushSystemFront(
			S* system)
		{
			LOG_INFO << "Pushed " << system->Name() << " system onto " << Name() << " layer";
			system->SetAppVars(MakeAppVars());
			m_systems.PushFront(system);

			system->OnPush();
		}

		template<
			typename S>
		void PopSystem(
			S* system)
		{
			LOG_INFO << "Popped " << system->Name() << " system from " << Name() << " layer";
			m_systems.Pop(system);

			system->OnPop();
		}

		iw::Scene* GetMainScene() {
			return MainScene;
		}
	protected:
		inline EventSequence CreateSequence() {
			EventSequence seq;
			seq.SetAppVars(MakeAppVars());

			return seq;
		}
	private:
		friend class Application;

		SET_APP_VARS
		MAKE_APP_VARS
	};
}

	using namespace Engine;
}
