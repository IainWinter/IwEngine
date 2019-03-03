#include "Application.h"
#include <iostream>

namespace IwEngine {
	Application::Application()
		: m_window(Window::Create())
		, window(*m_window)
	{}

	Application::~Application() {
		delete m_window;
	}

	void Application::Initilize() {
		window.SetCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		window.Initilize();
	}

	void Application::Start() {
		m_running = true;
		Run();
	}

	void Application::Stop() {
		m_running = false;
	}

	void Application::Destroy() {
		window.Destroy();
	}

	void Application::Run() {
		while (m_running) {
			window.Update();
		}
	}

	void Application::OnEvent(Event& e) {
		if (e.type == WM_DESTROY) {
			Stop();
		}

		else if (e.type == WM_CLOSE) {
			Destroy();
			e.handled = true;
		}
	}
}
