#pragma once

#include <functional>
#include "Event.h"

namespace IwEngine {
	class Application;

	class Window {
	public:
		using EventCallback = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual int Initilize() = 0; // Creates a window
		virtual void Show()     = 0; // Opens a window
		virtual void Minimize() = 0; // Minimizes a window
		virtual void Destroy()  = 0; // Destorys a window
		virtual void Update()   = 0; // Polls for events

		virtual void DrawCursor(
			bool show) = 0;

		virtual void SetCallback(
			EventCallback callback) = 0;

		static Window* Create();
	};
}
