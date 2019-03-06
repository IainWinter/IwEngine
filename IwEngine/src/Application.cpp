#include "Application.h"
#include "logger.h"

namespace IwEngine {
	Application::Application()
		: m_running(false)
		, m_window (Window::Create())
		, window   (*m_window)
	{}

	Application::~Application() {
		delete m_window;
	}

	void Application::PushLayer(
		Layer* layer)
	{
		m_layerStack.PushLayer(layer);
	}

	void Application::PushOverlay(
		Layer* overlay)
	{
		m_layerStack.PushOverlay(overlay);
	}

	void Application::PopLayer(
		Layer* layer)
	{
		m_layerStack.PopLayer(layer);

	}

	void Application::PopOverlay(
		Layer* overlay)
	{
		m_layerStack.PopOverlay(overlay);
	}

	int Application::Initilize(
		WindowOptions& windowOptions)
	{
		LOG_SINK(iwlog::stdout_sink, iwlog::INFO);
		LOG_SINK(iwlog::stderr_sink, iwlog::ERR);
		LOG_SINK(iwlog::file_sink, iwlog::DEBUG, "sandbox.log");

		LOG_DEBUG << "Initilizing application";

		window.SetCallback(std::bind(&Application::OnEvent, this, std::placeholders::_1));
		int status;
		if (status = window.Initilize(windowOptions)) {
			LOG_ERROR << "Window failed to initilize with error code " << status;
			return status;
		}

		for (Layer* layer : m_layerStack) {
			if (status = layer->Initilize()) {
				LOG_ERROR << layer->Name() << " layer failed to initilize with error code " << status;
				return status;
			}
		}

		return 0;
	}

	void Application::Start() {
		LOG_DEBUG << "Starting application";
		m_running = true;

		for (Layer* layer : m_layerStack) {
			layer->Start();
		}

		Run();
	}

	void Application::Stop() {
		LOG_DEBUG << "Stopping application";
		m_running = false;

		for (Layer* layer : m_layerStack) {
			layer->Stop();
		}
	}

	void Application::Destroy() {
		LOG_DEBUG << "Destroying application";
		window.Destroy();

		for (Layer* layer : m_layerStack) {
			layer->Destroy();
		}
	}

	void Application::Run() {
		LOG_DEBUG << "Running application...";
		while (m_running) {
			for (Layer* layer : m_layerStack) {
				layer->Update();
			}

			window.Update();
		}
	}

	void Application::OnEvent(Event& e) {
		for (Layer* layer : m_layerStack) {
			layer->OnEvent(e);
		}

		if (e.type == WM_CLOSE) {
			Stop();
			Destroy();
		}
	}
}
