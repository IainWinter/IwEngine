#pragma once

#include <Windows.h>
#include "Window.h"

namespace IwEngine {
	class WindowsWindow
		: public Window
	{
	public:
	private:
		HINSTANCE m_handle;
		bool console;

	public:
		WindowsWindow() {
			m_handle = GetModuleHandle(NULL); //this probaly work but maybe not
		}

		int Open();
		void Close();

	private:

	};
}
