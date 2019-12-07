#pragma once

#include "iw/engine/Window.h"
#include "iw/input/InputState.h"
#include <Windows.h>

namespace IW {
namespace Engine {
	class WindowsWindow
		: public Window
	{
	private:
		HINSTANCE m_instance;
		HWND      m_window;
		HDC       m_device;
		HGLRC     m_context;

	public:
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

		static LRESULT CALLBACK _WndProc(
			HWND hwnd,
			UINT msg,
			WPARAM wparam,
			LPARAM lparam);
	private:
		LRESULT CALLBACK HandleEvent(
			HWND hwnd,
			UINT msg,
			WPARAM wparam,
			LPARAM lparam);
	};
}

	using namespace Engine;
}
