#pragma once

#include "Events/Event.h"
#include "iw/events/functional/callback.h"
#include "iw/input/InputManager.h"

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
		bool         cursor;
		DisplayState state;
	};

	class IWindow {
	public:
		using EventCallback = iwevents::callback<Event&>;

		virtual ~IWindow() {}

		virtual int Initialize(
			const WindowOptions& options) = 0;

		virtual void Destroy() = 0;
		virtual void Update()  = 0;
		virtual void Render()  = 0;
		virtual void Clear()   = 0;

		virtual void SetInputManager(
			IwInput::InputManager& inputManager) = 0;

		virtual void SetState(
			DisplayState state) = 0;

		virtual void SetCursor(
			bool show) = 0;

		virtual void SetDimensions(
			unsigned int width,
			unsigned int height) = 0;

		virtual void SetCallback(
			EventCallback callback) = 0;

		virtual unsigned int Id()     = 0;
		virtual unsigned int Width()  = 0;
		virtual unsigned int Height() = 0;
		virtual bool         Cursor() = 0;
		virtual DisplayState State()  = 0;

		static IWindow* Create();
	};

	class Window
		: public IWindow
	{
	protected:
		EventCallback callback;
		WindowOptions options;
		IwInput::InputManager* inputManager;

	public:
		virtual ~Window() {}

		inline void SetCallback(
			EventCallback callback)
		{
			this->callback = callback;
		}

		inline unsigned int Id() {
			static unsigned int nextId = 0;
			static unsigned int id = nextId++;
			return id;
		}

		inline unsigned int Width() {
			return options.width;
		}

		inline unsigned int Height() {
			return options.height;
		}

		inline bool Cursor() {
			return options.cursor;
		}

		inline DisplayState State() {
			return options.state;
		}

	};
}
