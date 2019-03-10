#pragma once

#include "Event.h"
#include "iw/events/functional/callback.h"

namespace IwEngine {
	enum DisplayState {
		NORMAL,
		HIDDEN,
		MINIMIZED,
		MAXIMIZED,
		FULLSCREEN, //Don't work
		BORDERLESS // Don't work
	};

	struct WindowOptions {
		unsigned int width;
		unsigned int height;
		DisplayState state;
		bool cursor;
	};

	class Window {
	public:
		using EventCallback = iwevents::callback<Event&>;

		virtual ~Window() {}

		virtual int Initilize(
			WindowOptions& options) = 0;

		virtual void Destroy() = 0;
		virtual void Update()  = 0;

		virtual void SetDisplayState(
			DisplayState state) = 0;

		virtual void SetCallback(
			EventCallback callback) = 0;

		virtual void DrawCursor(
			bool show) = 0;

		virtual unsigned int Width()        = 0;
		virtual unsigned int Height()       = 0;
		virtual DisplayState DisplayState() = 0;
		virtual bool Cursor()               = 0;

		virtual void Render() = 0;

		static Window* Create();
	};
}
