#pragma once

#include "Core.h"
#include "Events/AllEvents.h"

namespace IwEngine {
	class IWENGINE_API Layer {
	private:
		const char* m_name;

	public:
		Layer(
			const char* name);

		virtual ~Layer();

		virtual int  Initialize();
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
	};
}
