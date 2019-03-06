#pragma once

#include <functional>
#include "Event.h"

namespace IwEngine {
	enum DisplayState {
		NORMAL,
		HIDDEN,
		MINIMIZED,
		MAXIMIZED,
		FULLSCREEN, //Dont work
		BORDERLESS // Dont work
	};

	struct WindowOptions {
		unsigned int width;
		unsigned int height;
		DisplayState state;
		bool cursor; 
	};

	class Window {
	public:
		using EventCallback = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual int  Initilize(WindowOptions& options) = 0;
		virtual void Destroy() = 0;
		virtual void Update()  = 0;

		virtual void SetDisplayState(DisplayState state) = 0;
		virtual void SetCallback(EventCallback callback) = 0;
		virtual void DrawCursor(bool show)               = 0;

		virtual unsigned int Width()        = 0;
		virtual unsigned int Height()       = 0;
		virtual DisplayState DisplayState() = 0;
		virtual bool Cursor()               = 0;

		virtual void Render() = 0;

		static Window* Create();
	};
}
