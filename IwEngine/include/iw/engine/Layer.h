#pragma once

#include "Core.h"
#include "Stack.h"
#include "System.h"
#include "InitOptions.h"
#include "Events/AllEvents.h"
#include "iw/entity/Space.h"

namespace IwEngine {
	class IWENGINE_API Layer {
	private:
		const char* m_name;
		Stack<ISystem*> m_systems;
	protected:
		IwEntity::Space& Space;

	protected:
		void UpdateSystems();
	public:
		Layer(
			IwEntity::Space& space,
			const char* name);

		virtual ~Layer();

		virtual int Initialize(
			InitOptions& options);

		virtual void Destroy();
		virtual void Update();
		virtual void FixedUpdate();
		virtual void ImGui();

		virtual bool On(WindowResizedEvent& event);
		virtual bool On(MouseWheelEvent&    event);
		virtual bool On(MouseMovedEvent&    event);
		virtual bool On(MouseButtonEvent&   event);
		virtual bool On(KeyEvent&           event);
		virtual bool On(KeyTypedEvent&      event);

		inline const char* Name() {
			return m_name;
		}

		template<
			typename S,
			typename... Args>
		S* PushSystem(
			Args&& ... args)
		{
			S* layer = new S(Space, std::forward<Args>(args)...);
			m_systems.PushBack(layer);
			return layer;
		}

		template<
			typename S>
		void PopSystem(
			S* system)
		{
			m_systems.Pop(system);
		}
	};
}
