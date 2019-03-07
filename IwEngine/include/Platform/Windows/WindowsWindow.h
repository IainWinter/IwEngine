#pragma once

#include <Windows.h>
#include "Window.h"

namespace IwEngine {
	class WindowsWindow
		: public Window
	{
	private:
		HINSTANCE     m_instance;
		HWND          m_window;
		HDC           m_device;
		HGLRC         m_context;
		EventCallback m_callback;
		WindowOptions m_options;

	public:
		WindowsWindow() {
			m_instance = GetModuleHandle(NULL);
		}

		int Initilize(
			WindowOptions& options) override;

		void Destroy() override;
		void Update()  override;

		void SetDisplayState(
			IwEngine::DisplayState state) override;

		void SetCallback(
			EventCallback callback) override;

		void DrawCursor(
			bool show) override;

		void Render();

		inline unsigned int Width() {
			return m_options.width;
		}

		inline unsigned int Height() {
			return m_options.height;
		}

		inline IwEngine::DisplayState DisplayState() {
			return m_options.state;
		}

		inline bool Cursor() {
			return m_options.cursor;
		}

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
