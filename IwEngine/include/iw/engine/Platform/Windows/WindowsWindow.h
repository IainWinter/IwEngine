#pragma once

#include "iw/engine/Window.h"
#include <Windows.h>

namespace IwEngine {
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
		void Render()  override;
		void Clear()   override;

		void SetInputManager(
			IwInput::InputManager& manager);

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

		void HandleMouseWheel(
			float delta);

		void HandleMouseMoved(
			float X,
			float Y,
			float deltaX,
			float deltaY);

		void HandleMouseButton(
			IwInput::InputName button,
			bool down);

		void HandleKey(
			IwInput::InputName key,
			bool down);

		void HandleKeyTyped(
			IwInput::InputName key,
			char character);
	};
}
