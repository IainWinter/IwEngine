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

		virtual int  Initilize();
		virtual void Destroy();
		virtual void Update();

		virtual bool On(WindowResizedEvent&       event);
		virtual bool On(MouseMovedEvent&          event);
		virtual bool On(MouseButtonPressedEvent&  event);
		virtual bool On(MouseButtonReleasedEvent& event);

		inline const char* Name() {
			return m_name;
		}
	};
}
