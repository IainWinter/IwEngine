#include "iw/engine/Application.h"
#include "iw/log/logger.h"
#include "iw/log/sink/async_std_sink.h"
#include "iw/log/sink/file_sink.h"
#include "iw/events/functional/callback.h"
#include "iw/engine/Time.h"

namespace IwEngine {
	Application::Application()
		: m_window (IWindow::Create())
		, m_running(false)
	{}

	Application::~Application() {
		delete m_window;
	}

	int Application::Initialize(
		InitOptions& options)
	{
		Time::Update();

		LOG_SINK(iwlog::async_stdout_sink, iwlog::INFO);
		LOG_SINK(iwlog::async_stderr_sink, iwlog::ERR);
		LOG_SINK(iwlog::file_sink,         iwlog::DEBUG, "sandbox.log");

		m_window->SetCallback(
			iwevents::make_callback(&Application::HandleEvent, this));

		int status;
		LOG_DEBUG << "Initializing window...";
		if (status = m_window->Initialize(options.WindowOptions)) {
			LOG_ERROR << "Window failed to initialize with error code "
				<< status;
			return status;
		}

		LOG_DEBUG << "Binding input manager to window...";

		m_window->SetInputManager(InputManager);

		m_imguiLayer = new ImGuiLayer();
		m_layerStack.PushOverlay(m_imguiLayer);

		for (Layer* layer : m_layerStack) {
			LOG_DEBUG << "Initializing " << layer->Name() << " layer...";
			if (status = layer->Initialize(options)) {
				LOG_ERROR << layer->Name()
					<< " layer initialization failed with error code "
					<< status;
				return status;
			}
		}

		//Need to set after so window doesn't send events before imgui gets initialized 
		m_window->SetState(options.WindowOptions.State);

		Time::Update();
		Time::SetFixedTime(1 / 60.0f);

		return 0;
	}

	void Application::Run() {
		m_running = true;

		float accumulatedTime = 0;
		while (m_running) {
			Time::Update();

			accumulatedTime += Time::DeltaTime();
			while (accumulatedTime >= Time::FixedTime()) {
				FixedUpdate();
				accumulatedTime -= Time::FixedTime();
			}

			Update();
		}
	}

	void Application::Update() {
		m_window->Clear();

		for (Layer* layer : m_layerStack) {	
			layer->Update();
		}

		m_imguiLayer->Begin();
		for (Layer* layer : m_layerStack) {
			layer->ImGui();
		}
		m_imguiLayer->End();

		m_window->Render();
		m_window->Update();
	}

	void Application::FixedUpdate() {
		for (Layer* layer : m_layerStack) {
			layer->FixedUpdate();
		}
	}

	void Application::Destroy() {
		m_running = false;
		m_window->Destroy();
		for (Layer* layer : m_layerStack) {
			layer->Destroy();
		}
	}

	void Application::HandleEvent(
		Event& e)
	{
		switch (e.Type) {
			case WindowResized:    DispatchEvent((WindowResizedEvent&)e); break;
			case MouseWheel:       DispatchEvent((MouseWheelEvent&)e);    break;
			case MouseMoved:       DispatchEvent((MouseMovedEvent&)e);    break;
			case MouseButton:      DispatchEvent((MouseButtonEvent&)e);   break;
			case Key:    	       DispatchEvent((KeyEvent&)e);           break;
			case KeyTyped:         DispatchEvent((KeyTypedEvent&)e);      break;
			case WindowClosed:     Destroy();                                 break;
			default: LOG_WARNING << "Application mishandled event " + e.Type; break;
		}
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
}
