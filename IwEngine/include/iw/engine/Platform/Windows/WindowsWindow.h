#pragma once

#include "iw/engine/Window.h"
#include "iw/input/InputState.h"
#include <Windows.h>

namespace IW {
inline namespace Engine {
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

		void SetEventbus(
			iw::ref<iw::eventbus>& bus) override;

		void SetInputManager(
			iw::ref<InputManager>& manager) override;

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
			IW::InputState* inputState,
			float delta);

		void HandleMouseMoved(
			IW::InputState* inputState,
			float X,
			float Y,
			float deltaX,
			float deltaY);

		void HandleMouseButton(
			IW::InputState* inputState,
			IW::InputName button,
			bool down);

		void HandleKey(
			IW::InputState* inputState,
			IW::InputName key,
			bool down);

		void HandleKeyTyped(
			IW::InputState* inputState,
			IW::InputName key,
			char character);
	};
}
}
