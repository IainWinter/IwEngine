#include "iw/engine/Platform/Windows/WindowsWindow.h"
#include "EventTranslator.h"
#include "iw/log/logger.h"
#include "gl/glew.h"
#include "gl/wglew.h"
#include <cstdio>
#include <Tchar.h>

ATOM RegClass(
	HINSTANCE instance,
	WNDPROC wndproc);

namespace IwEngine {
	IWindow* IWindow::Create() {
		return new WindowsWindow();
	}

	int WindowsWindow::Initilize(
		const WindowOptions& options)
	{
		m_instance = GetModuleHandle(NULL);
		MAKEINTATOM(RegClass(m_instance, _WndProc));

		HWND fake_hwnd = CreateWindow(
			_T("Core"), _T("Fake Window"),
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			0, 0,
			1, 1,
			NULL, NULL,
			m_instance, NULL);

		HDC fake_dc = GetDC(fake_hwnd);

		PIXELFORMATDESCRIPTOR fake_pfd;
		ZeroMemory(&fake_pfd, sizeof(fake_pfd));
		fake_pfd.nSize = sizeof(fake_pfd);
		fake_pfd.nVersion = 1;
		fake_pfd.dwFlags = PFD_DRAW_TO_WINDOW
			| PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		fake_pfd.iPixelType = PFD_TYPE_RGBA;
		fake_pfd.cColorBits = 32;
		fake_pfd.cAlphaBits = 8;
		fake_pfd.cDepthBits = 24;

		INT fake_pfdId = ChoosePixelFormat(fake_dc, &fake_pfd);
		if (fake_pfdId == 0) {
			LOG_ERROR << "ChoosePixelFormat() failed!";
			return 1;
		}

		if (!SetPixelFormat(fake_dc, fake_pfdId, &fake_pfd)) {
			LOG_ERROR << "SetPixelFormat() failed!";
			return 1;
		}

		HGLRC fake_rc = wglCreateContext(fake_dc);
		if (fake_rc == 0) {
			LOG_ERROR << "wglCreateContext() failed!";
			return 1;
		}

		if (!wglMakeCurrent(fake_dc, fake_rc)) {
			LOG_ERROR << "wglMakeCurrent() failed!";
			return 1;
		}

		if (glewInit() != GLEW_OK) {
			LOG_ERROR << "glewInit() failed!";
			return 1;
		}

		m_window = CreateWindow(
			_T("Core"), _T("Space"),
			WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
			100, 100,
			options.width, options.height,
			NULL, NULL,
			m_instance, NULL);

		SetWindowLongPtr(m_window, GWLP_USERDATA, (LONG_PTR)this);

		m_device = GetDC(m_window);

		CONST INT pixelAttribs[] = {
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

		INT pfid;
		UINT formatCount;
		BOOL status = wglChoosePixelFormatARB(m_device, pixelAttribs,
			NULL, 1, &pfid, &formatCount);

		if (status == FALSE || formatCount == 0) {
			LOG_ERROR << "wglChoosePixelFormatARB() failed!";
			return 1;
		}

		PIXELFORMATDESCRIPTOR pfd;
		DescribePixelFormat(m_device, pfid, sizeof(pfd), &pfd);
		SetPixelFormat(m_device, pfid, &pfd);

		CONST INT major = 4, minor = 5;
		INT contextAttribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, major,
			WGL_CONTEXT_MINOR_VERSION_ARB, minor,
			WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0
		};

		m_context = wglCreateContextAttribsARB(m_device, 0, contextAttribs);
		if (m_context == NULL) {
			LOG_ERROR << "wglCreateContextAttribsARB() failed!";
			return 1;
		}

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(fake_rc);
		ReleaseDC(fake_hwnd, fake_dc);
		DestroyWindow(fake_hwnd);
		if (!wglMakeCurrent(m_device, m_context)) {
			LOG_ERROR << "wglMakeCurrent() failed.";
			return 1;
		}

		SetCursor(options.cursor);

		this->options = options;

		return 0;
	}

	void WindowsWindow::Destroy() {
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_context);
		ReleaseDC(m_window, m_device);
		DestroyWindow(m_window);
	}

	void WindowsWindow::Update() {
		MSG msg;
		while (PeekMessage(&msg, m_window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	void WindowsWindow::Render() {
		SwapBuffers(m_device);
	}

	void WindowsWindow::SetInputManager(
		IwInput::InputManager& manager)
	{
		inputManager = &manager;

		manager.CreateContext(Id(), Width(), Height());

		manager.SetMouseWheelCallback(Id(), 
			iwevents::make_callback(&WindowsWindow::HandleMouseWheel, this));

		manager.SetMouseMovedCallback(Id(), 
			iwevents::make_callback(&WindowsWindow::HandleMouseMoved, this));

		manager.SetMouseButtonCallback(Id(), 
			iwevents::make_callback(&WindowsWindow::HandleMouseButton, this));

		manager.SetKeyCallback(Id(),         
			iwevents::make_callback(&WindowsWindow::HandleKey, this));
	}

	void WindowsWindow::SetState(
		DisplayState state)
	{
		int wstate = -1;
		switch (state) {
			case HIDDEN:    wstate = SW_HIDE;       break;
			case MINIMIZED: wstate = SW_MINIMIZE;   break;
			case MAXIMIZED: wstate = SW_MAXIMIZE;   break;
			case NORMAL:    wstate = SW_SHOWNORMAL; break;
		}

		if (wstate != -1) {
			ShowWindow(m_window, wstate);
		}

		options.state = state;
	}


	void WindowsWindow::SetCursor(
		bool show)
	{
		ShowCursor(show);
		options.cursor = show;
	}

	LRESULT CALLBACK WindowsWindow::_WndProc(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam)
	{
		WindowsWindow* me = reinterpret_cast<WindowsWindow*>(
			GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (me) {
			return me->HandleEvent(hwnd, msg, wParam, lParam);
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	LRESULT CALLBACK WindowsWindow::HandleEvent(
		HWND hwnd,
		UINT msg,
		WPARAM wParam,
		LPARAM lParam)
	{
		//Input events
		inputManager->HandleEvent(Id(), msg, wParam, lParam);
		
		Event* e;
		switch (msg) {
			case WM_SIZE:
				e = &Translate<WindowResizedEvent>(msg, wParam, lParam);
				break;
			case WM_CLOSE:
				e = &Event(WindowClosed);
				break;
			case WM_DESTROY:
				e = &Event(WindowDestroyed);
				break;
			default:
				e = &Event(NOT_HANDLED);
				break;
		}

		if (e->Type == NOT_HANDLED) {
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		callback(*e);

		if (!e->Handled) {
			return DefWindowProc(hwnd, msg, wParam, lParam);
		}

		return 0;
	}

	void WindowsWindow::HandleMouseWheel(
		float delta)
	{
		MouseWheelEvent e(delta);
		callback(e);

		LOG_INFO << "Mouse wheel moved " << delta;
	}

	void WindowsWindow::HandleMouseMoved(
		float X, 
		float Y,
		float deltaX,
		float deltaY)
	{
		MouseMovedEvent e(X, Y, deltaX, deltaY);
		callback(e);

		LOG_INFO << "Mouse moved " << deltaX << ", " << deltaY 
			<< " to " << X << ", " << Y;
	}
	
	void WindowsWindow::HandleMouseButton(
		IwInput::InputName button,
		bool down)
	{
		MouseButtonEvent e(button, down);
		callback(e);

		LOG_INFO << "Mouse button " << button <<
			(down ? " pressed" : " released");
	}

	void WindowsWindow::HandleKey(
		IwInput::InputName button,
		bool down)
	{
		LOG_INFO << "Key " << button <<
			(down ? " pressed" : " released");
	}
}

ATOM RegClass(
	HINSTANCE instance,
	WNDPROC wndproc)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = CS_HREDRAW | CS_OWNDC;
	wcex.lpfnWndProc = wndproc;
	wcex.hInstance = instance;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszClassName = _T("Core");

	return RegisterClassEx(&wcex);
}
