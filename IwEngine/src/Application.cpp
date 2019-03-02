#include "Application.h"

namespace IwEngine {
	Application::Application()
		: m_window(Window::Create())
		, window(*m_window)
	{}

	Application::~Application() {
		delete m_window;
	}

	void Application::Initilize() {
		window.Open();
	}

	void Application::Start() {
		
	}

	void Application::Stop() {

	}

	void Application::Destroy() {
		window.Close();
	}
}
