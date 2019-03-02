#include "Platform/Windows/WindowsWindow.h"
#include "gl/glew.h"
#include "gl/wglew.h"
#include <cstdio>
#include <iostream>

LRESULT CALLBACK WinProc(
	HWND hwnd,
	UINT msg,
	WPARAM wparam,
	LPARAM lparam);

ATOM RegClass(
	HINSTANCE instance);

namespace IwEngine {
	Window* Window::Create() {
		return new WindowsWindow();
	}

	int WindowsWindow::Open() {
		//if (console) {
		//	AllocConsole();
		//	FILE* fp;
		//	freopen_s(&fp, "CONOUT$", "w", stdout);
		//}

		LPTSTR window = MAKEINTATOM(RegClass(m_handle));

		HWND fake_h_wnd = CreateWindow(
			"Core", "Fake Window",
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0, 0,
			1, 1,
			NULL, NULL,
			m_handle, NULL);

		HDC fake_dc = GetDC(fake_h_wnd);

		PIXELFORMATDESCRIPTOR fake_pfd;
		ZeroMemory(&fake_pfd, sizeof(fake_pfd));
		fake_pfd.nSize = sizeof(fake_pfd);
		fake_pfd.nVersion = 1;
		fake_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		fake_pfd.iPixelType = PFD_TYPE_RGBA;
		fake_pfd.cColorBits = 32;
		fake_pfd.cAlphaBits = 8;
		fake_pfd.cDepthBits = 24;

		int fake_pfd_id = ChoosePixelFormat(fake_dc, &fake_pfd);
		if (fake_pfd_id == 0) {
			std::cout << "ChoosePixelFormat() failed!";
			return 1;
		}

		if (!SetPixelFormat(fake_dc, fake_pfd_id, &fake_pfd)) {
			std::cout << "SetPixelFormat() failed!";
			return 1;
		}

		HGLRC fake_rc = wglCreateContext(fake_dc);
		if (fake_rc == 0) {
			std::cout << "wglCreateContext() failed!";
			return 1;
		}

		if (!wglMakeCurrent(fake_dc, fake_rc)) {
			std::cout << "wglMakeCurrent() failed!";
			return 1;
		}

		if (glewInit() != GLEW_OK) {
			std::cout << "glewInit() failed!";
			return 1;
		}

		HWND h_wnd = CreateWindow(
			"Core", "Space",
			WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			100, 100,
			1280, 720,
			NULL, NULL,
			m_handle, NULL);

		HDC dc = GetDC(h_wnd);

		const int pixelAttribs[] = {
			WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
			WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
			WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
			WGL_COLOR_BITS_ARB, 32,
			WGL_ALPHA_BITS_ARB, 8,
			WGL_DEPTH_BITS_ARB, 24,
			WGL_STENCIL_BITS_ARB, 8,
			WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
			WGL_SAMPLES_ARB, 4,
			0
		};

		int pfid;
		UINT format_count;
		bool status = wglChoosePixelFormatARB(dc, pixelAttribs, NULL, 1, &pfid, &format_count);

		if (status == false || format_count == 0) {
			std::cout << "wglChoosePixelFormatARB() failed!";
			return 1;
		}

		PIXELFORMATDESCRIPTOR pfd;
		DescribePixelFormat(dc, pfid, sizeof(pfd), &pfd);
		SetPixelFormat(dc, pfid, &pfd);

		const int major = 4, minor = 5;
		int context_attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, major,
			WGL_CONTEXT_MINOR_VERSION_ARB, minor,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		HGLRC rc = wglCreateContextAttribsARB(dc, 0, context_attribs);
		if (rc == NULL) {
			std::cout << "wglCreateContextAttribsARB() failed!";
			return 1;
		}

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(fake_rc);
		ReleaseDC(fake_h_wnd, fake_dc);
		DestroyWindow(fake_h_wnd);
		if (!wglMakeCurrent(dc, rc)) {
			std::cout << "wglMakeCurrent() failed.";
			return 1;
		}

		ShowWindow(h_wnd, 0);
		ShowCursor(0);

		MSG msg;
		while (PeekMessage(&msg, h_wnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void WindowsWindow::Close() {
		//if (console) {
		//	fclose(fp);
		//	FreeConsole();
		//}
	}
}

LRESULT CALLBACK WinProc(
	HWND hwnd,
	UINT msg,
	WPARAM wparam,
	LPARAM lparam)
{
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(wparam);
		break;
	case WM_CLOSE:
		break;
	default:
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return 0;
}

ATOM RegClass(
	HINSTANCE instance)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = WinProc;
	wcex.hInstance = instance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = "Core";

	return RegisterClassEx(&wcex);
}
