#include "iw/engine/Application.h"
#include "iw/engine/Layers/DebugLayer.h"
#include "iw/engine/Time.h"
#include "iw/log/logger.h"
#include "iw/log/sink/async_std_sink.h"
#include "iw/log/sink/file_sink.h"
#include "iw/events/callback.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/graphics/Loaders/MaterialLoader.h"
#include "iw/graphics/Loaders/TextureLoader.h"
#include "iw/graphics/Loaders/ShaderLoader.h"
#include <atomic>

namespace IW {
	Application::Application()
		: m_running(false)
		, m_window(IWindow::Create())
		, m_device(IW::IDevice::Create())
		, Renderer(m_device)
		, m_updateTask([&]() {
			for (Layer* layer : m_layers) {
				layer->UpdateSystems();
			}
		})
	{
		m_imguiLayer = PushOverlay<ImGuiLayer>();
		PushOverlay<DebugLayer>();
	}

	Application::~Application() {
		delete m_window;
	}

	int Application::Initialize(
		InitOptions& options)
	{
		// Time

		Time::UpdateTime();

		// Logging

		LOG_SINK(iw::async_stdout_sink, iw::INFO);
		LOG_SINK(iw::async_stderr_sink, iw::ERR);
		LOG_SINK(iw::file_sink,         iw::DEBUG, "sandbox.log");

		// Events

		Bus.subscribe(iw::make_callback(&Application::HandleEvent, this));

		// Asset Loader

		Asset.SetLoader<IW::MeshLoader>();
		Asset.SetLoader<IW::MaterialLoader>();
		Asset.SetLoader<IW::TextureLoader>();
		Asset.SetLoader<IW::ShaderLoader>();

		// Window

		LOG_DEBUG << "Setting window event bus...";
		m_window->SetEventbus(Bus);
		
		LOG_DEBUG << "Setting window input manager...";
		m_window->SetInputManager(InputManager);

		int status;
		LOG_DEBUG << "Initializing window...";
		if (status = m_window->Initialize(options.WindowOptions)) {
			LOG_ERROR << "Window failed to initialize with error code " << status;
			return status;
		}

		Renderer.Initialize();

		for (Layer* layer : m_layers) {
			LOG_DEBUG << "Initializing " << layer->Name() << " layer...";
			if (status = layer->Initialize(options)) {
				LOG_ERROR << layer->Name() << " layer initialization failed with error code " << status;
				return status;
			}
		}

		//Need to set after so window doesn't send events before imgui gets initialized 
		m_window->SetState(options.WindowOptions.State);

		// Time again!

		Time::UpdateTime();
		Time::SetFixedTime(1 / 60.0f);

		return 0;
	}

	void Application::Run() {
		Bus.publish();
		m_window->ReleaseOwnership();

		m_running = true;

		// RenderAPI thread is the 'main' thread for the application (owns rendering context)
		m_renderThread = std::thread([&]() {
			m_window->TakeOwnership();
			
			float accumulatedTime = 0;
			while (m_running) {
				Update();

				if (Time::FixedTime() == 0)
					continue;

				accumulatedTime += Time::DeltaTime();
				while (m_running 
					&& accumulatedTime >= Time::FixedTime())
				{
					FixedUpdate();
					accumulatedTime -= Time::FixedTime();
				}
			}
		});

		// Main loop gets events from os
		while (m_running) {
			m_window->Update();
		}

		m_renderThread.join();
		
		Destroy();
	}

	void Application::Update() {
		// Update time (Sync)
		Time::UpdateTime();

		Renderer.Begin();

		// Clear window and poll events (Sync)
		//m_window->Clear();

		//m_workQueue.clear();

		// Pre Update (Sync)
		for (Layer* layer : m_layers) {
			layer->PreUpdate();
		}

		// Start Work (ASync)
		//m_updateTask.Run();

		// Update layers (ASync)
		for (Layer* layer : m_layers) {
			layer->UpdateSystems();
			layer->Update();
		}

		// Pause until work is finished (ASync)
		//m_updateTask.Wait();

		// Post Update (Sync)
		for (Layer* layer : m_layers) {
			layer->PostUpdate();
		}

		// ImGui render (Sync)
		m_imguiLayer->Begin();
		for (Layer* layer : m_layers) {
			layer->ImGui();
		}
		m_imguiLayer->End();

		// Run through render queue! (Sync)

		Renderer.End();

		// Swap buffers (Sync)
		m_window->SwapBuffers();
		Bus.publish();
	}

	void Application::FixedUpdate() {
		for (Layer* layer : m_layers) { //Need fixed update for systems too
			layer->FixedUpdateSystems();
			layer->FixedUpdate();
		}
	}

	void Application::Destroy() {
		m_window->Destroy();
		for (Layer* layer : m_layers) {
			layer->Destroy();
		}
	}

	void Application::HandleEvent(
		iw::event& e)
	{
		bool error = false;
		if (e.Category == IW::WINDOW) {
			switch (e.Type) {
				case IW::Closed: {
					m_running = false;
					e.Handled = true;
					break;
				}
				case IW::Resized: {
					Renderer.Width  = e.as<IW::WindowResizedEvent>().Width;
					Renderer.Height = e.as<IW::WindowResizedEvent>().Height;
					e.Handled = true;
					break;
				}
				default: error = true; break;
			}
		}

		if (error) {
			LOG_WARNING << "Application mishandled event " + e.Type << "!";
		}

		else {
			m_layers.DispatchEvent(e);
		}
	}
}
