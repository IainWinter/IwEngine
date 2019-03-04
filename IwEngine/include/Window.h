#pragma once

#include <functional>
#include "Event.h"

namespace IwEngine {
	enum DisplayState {
		NORMAL,
		HIDDEN,
		MINIMIZED,
		MAXIMIZED,
		FULLSCREEN,
		BORDERLESS
	};

	class Window {
	public:
		using EventCallback = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual int Initilize() = 0; // Creates a window
		virtual void Destroy()  = 0; // Destorys a window
		virtual void Update()   = 0; // Polls for events

		virtual void SetDisplayState(
			DisplayState state) = 0;

		virtual void SetCallback(
			EventCallback callback) = 0;

		virtual void DrawCursor(
			bool show) = 0;

		static Window* Create();
	};
}
