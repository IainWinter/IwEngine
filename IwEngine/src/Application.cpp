#include "Application.h"
#include "log.h"

namespace IwEngine {
	Application::Application()
		: m_running(false)
		, m_window (Window::Create())
		, window   (*m_window)
	{}

	Application::~Application() {
		delete m_window;
	}

	int Application::Initilize() {
		LOG_START(iwlog::IW_DEBUG);

		window.SetCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		if (window.Initilize()) {
			LOG_ERROR("Window failed to initilize!");
			return 1;
		}

		return 0;
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
