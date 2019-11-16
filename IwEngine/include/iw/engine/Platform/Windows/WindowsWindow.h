#pragma once

#include "iw/engine/Window.h"
#include "iw/input/InputState.h"
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
		//void Clear()   override;

		void SwapBuffers() override;
		void PollEvents()  override;

		bool TakeOwnership()    override;
		bool ReleaseOwnership() override;

		void SetEventbus(
			iw::eventbus& bus) override;

		void SetInputManager(
			IwInput::InputManager& manager) override;

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
			IwInput::InputState inputState,
			float delta);

		void HandleMouseMoved(
			IwInput::InputState inputState,
			float X,
			float Y,
			float deltaX,
			float deltaY);

		void HandleMouseButton(
			IwInput::InputState inputState,
			IwInput::InputName button,
			bool down);

		void HandleKey(
			IwInput::InputState inputState,
			IwInput::InputName key,
			bool down);

		void HandleKeyTyped(
			IwInput::InputState inputState,
			IwInput::InputName key,
			char character);
	};
}
