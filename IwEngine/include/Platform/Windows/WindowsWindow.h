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

	public:
		WindowsWindow() {
			m_instance = GetModuleHandle(NULL);
		}

		int Initilize()   override;
		void Destroy()    override;
		void Update()     override;

		virtual void SetDisplayState(
			DisplayState state) override;

		void SetCallback(
			EventCallback callback) override;

		void DrawCursor(
			bool show) override;

		static LRESULT CALLBACK _WndProc(
			HWND hwnd,
			UINT msg,
			WPARAM wparam,
			LPARAM lparam);
	private:
		LRESULT CALLBACK WndProc(
			HWND hwnd,
			UINT msg,
			WPARAM wparam,
			LPARAM lparam);
	};
}
