#pragma once

#include "Core.h"
#include "System.h"
#include "InitOptions.h"
#include "EventStack.h"
#include "iw/graphics/Scene.h"
#include "iw/common/Events/OSEvents.h"

#undef PlaySound

namespace iw {
namespace Engine {
	class Application;

	class Layer {
	private:
		const char* m_name;
		EventStack<ISystem*> m_systems; // layer doesnt own systems but prolly should
	protected:
		APP_VARS

		Scene* MainScene;

	public:
		IWENGINE_API
		Layer(
			const char* name);

		IWENGINE_API
		virtual ~Layer();

		IWENGINE_API
		virtual int Initialize();

		// Sync Updates

		IWENGINE_API
		virtual void Destroy();

		IWENGINE_API
		virtual void ImGui();

		IWENGINE_API
		virtual void PreUpdate();

		IWENGINE_API
		virtual void Update();

		IWENGINE_API
		virtual void PostUpdate();

		IWENGINE_API
		virtual void FixedUpdate();

		IWENGINE_API
		virtual void OnPush();

		IWENGINE_API
		virtual void OnPop();

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
		void UpdateSystems(std::unordered_map<const char*, float>& debugpoop, float smooth);

		IWENGINE_API
		void FixedUpdateSystems();

		inline const char* Name() {
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
			LOG_INFO << "Pushed " << system->Name() << " system onto " << Name() << " layer";
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
