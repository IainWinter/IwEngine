#pragma once

#include "iw/events/eventbus.h"
#include "WindowOptions.h"
#include "iw/input/InputManager.h"
#include "iw/util/memory/ref.h"

namespace iw {
namespace Engine {
	class IWindow {
	public:
		virtual ~IWindow() {}

		virtual int Initialize(
			const WindowOptions& options) = 0;

		virtual void Destroy() = 0;
		virtual void Update()  = 0;
		//virtual void Clear() =   0;

		virtual void SwapBuffers() = 0;

		virtual bool TakeOwnership()    = 0;
		virtual bool ReleaseOwnership() = 0;

		virtual void SetEventbus(
			iw::ref<iw::eventbus>& bus) = 0;

		virtual void SetState(
			DisplayState state) = 0;

		virtual void SetCursor(
			bool show) = 0;

		//virtual void SetDimensions(
		//	unsigned int width,
		//	unsigned int height) = 0;

		virtual unsigned int Id()     = 0;
		virtual unsigned int Width()  = 0;
		virtual unsigned int Height() = 0;
		virtual bool         Cursor() = 0;
		virtual DisplayState State()  = 0;

		virtual void* Handle()  const = 0;
		virtual void* Context() const = 0;

		static IWindow* Create();
	};

	class Window
		: public IWindow
	{
	protected:
		WindowOptions Options;
		ref<InputManager> Input;
		ref<eventbus> Bus;

	public:
		virtual ~Window() {}

		inline void SetEventbus(
			ref<eventbus>& bus) override
		{
			Bus = bus;
		}

		inline unsigned int Id() {
			static unsigned int nextId = 0;
			static unsigned int id = nextId++;
			return id;
		}

		inline unsigned int Width() {
			return Options.Width;
		}

		inline unsigned int Height() {
			return Options.Height;
		}

		inline bool Cursor() {
			return Options.Cursor;
		}

		inline DisplayState State() {
			return Options.State;
		}
	};
}

	using namespace Engine;
}
