#include "Application.h"
#include "logger.h"

namespace IwEngine {
	Application::Application()
		: m_running(false)
		, m_window (Window::Create())
		, window   (*m_window)
		, space    ("Root")
	{}

	Application::~Application() {
		delete m_window;
	}

	int Application::Initilize(
		WindowOptions& windowOptions)
	{
		LOG_SINK(iwlog::stdout_sink, iwlog::INFO);
		LOG_SINK(iwlog::stderr_sink, iwlog::ERR);
		LOG_SINK(iwlog::file_sink, iwlog::DEBUG, "sandbox.log");

		LOG_DEBUG << "Initilizing application";

		window.SetCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		if (window.Initilize(windowOptions)) {
			LOG_ERROR << "Window failed to initilize!";
			return 1;
		}

		return 0;
	}

	void Application::Start() {
		LOG_DEBUG << "Starting application";
		m_running = true;
		Run();
	}

	void Application::Stop() {
		LOG_DEBUG << "Stopping application";
		m_running = false;
	}

	void Application::Destroy() {
		LOG_DEBUG << "Destroying application";
		window.Destroy();
	}

	void Application::Run() {
		LOG_DEBUG << "Running application...";
		while (m_running) {
			window.Update();
			space.Update();
		}
	}

	void Application::OnEvent(Event& e) {
		LOG_INFO << "Event recieved - " << e.type;
		if (e.type == WM_CLOSE) {
			Stop();
			Destroy();
		}
	}
}
