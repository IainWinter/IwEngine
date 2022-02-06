#pragma once

#include "iw/engine/Window.h"
#include "iw/input/InputState.h"

namespace iw {
namespace Engine {
	class WindowsWindow
		: public Window
	{
	private:
		HINSTANCE m_instance;
		HWND      m_window;
		HDC       m_device;
		HGLRC     m_context;

		// For fullscreen
		WINDOWPLACEMENT m_placementPrevious;
		DisplayState m_actualState;

	public:
		WindowsWindow()
			: Window              ()
			, m_instance          (nullptr)
			, m_window            (nullptr)
			, m_device            (nullptr)
			, m_context           (nullptr)
			, m_placementPrevious ()
			, m_actualState       (DisplayState::HIDDEN)
		{}

		int Initialize(
			const WindowOptions& options) override;

		void Destroy() override;
		void Update()  override;
		//void Clear()   override;

		void SwapBuffers() override;

		bool TakeOwnership()    override;
		bool ReleaseOwnership() override;

		void SetState(
			DisplayState state) override;

		void SetCursor(
			bool show) override;

		void* Handle()   const override { return m_window; }
		void* Context()  const override { return m_context; }
		void* Device()   const override { return m_device; }
		void* Instance() const override { return m_instance; }

		LRESULT CALLBACK HandleEvent(
			HWND hwnd,
			UINT msg,
			WPARAM wparam,
			LPARAM lparam);
	};
}

	using namespace Engine;
}
