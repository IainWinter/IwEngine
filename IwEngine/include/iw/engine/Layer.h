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
		IWENGINE_API
		Layer(
			std::string name);

		IWENGINE_API
		virtual ~Layer();

		IWENGINE_API virtual int  Initialize();
		IWENGINE_API virtual void Destroy();

		// Sync Updates

		IWENGINE_API virtual void ImGui();

		IWENGINE_API virtual void PreUpdate();
		IWENGINE_API virtual void Update(); // this one should be in thread pool btw 'AsyncUpdate' is maybe a better name
		IWENGINE_API virtual void PostUpdate();

		IWENGINE_API virtual void FixedUpdate();

		IWENGINE_API virtual void OnPush();
		IWENGINE_API virtual void OnPop();

		// Action events

		IWENGINE_API virtual bool On(ActionEvent& e);

		// Input events

		IWENGINE_API virtual bool On(MouseWheelEvent& e);
		IWENGINE_API virtual bool On(MouseMovedEvent& e);
		IWENGINE_API virtual bool On(MouseButtonEvent& e);
		IWENGINE_API virtual bool On(KeyEvent& e);
		IWENGINE_API virtual bool On(KeyTypedEvent& e);

		// Window events

		IWENGINE_API virtual bool On(WindowResizedEvent& e);

		// Os events

		IWENGINE_API virtual bool On(OsEvent& e);

		// Physics events

		IWENGINE_API virtual bool On(CollisionEvent& e);

		// Entity events

		IWENGINE_API virtual bool On(EntityDestroyEvent& e);
		IWENGINE_API virtual bool On(EntityDestroyedEvent& e);

		// System updates

		IWENGINE_API
		void UpdateSystems();

		IWENGINE_API
		void FixedUpdateSystems();

		const std::string& Name() const {
			return m_name;
		}

		EventStack<ISystem*> temp_GetSystems() {
			return m_systems;
		}

		template<
			typename S>
		S* GetSystem(
			const char* name)
		{
			return (S*)m_systems.Get(name);
		}

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
