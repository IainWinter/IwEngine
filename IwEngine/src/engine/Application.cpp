#include "iw/engine/Application.h"
#include "iw/engine/Layers/DebugLayer.h"
#include "iw/engine/Time.h"
#include "iw/log/logger.h"
#include "iw/log/sink/std_sink.h"
#include "iw/log/sink/file_sink.h"
#include "iw/events/callback.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/graphics/Loaders/MaterialLoader.h"
#include "iw/graphics/Loaders/TextureLoader.h"
#include "iw/graphics/Loaders/ShaderLoader.h"
#include "iw/graphics/Loaders/FontLoader.h"
#include <atomic>

namespace iw {
namespace Engine {
	Application::Application()
		: m_running(false)
		, m_window(IWindow::Create())
	{
		m_device = iw::ref<IDevice>(IDevice::Create());
		Renderer = std::make_shared<iw::Renderer>(m_device);

		Bus   = std::make_shared<iw::eventbus>();
		Input = std::make_shared<InputManager>(Bus);

		Console = std::make_shared<iw::Console>(iw::make_getback(&Application::HandleCommand, this));

		Physics = std::make_shared<DynamicsSpace>();
		Space = std::make_shared<iw::Space>();
		Asset = std::make_shared<AssetManager>();

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

		LOG_SINK(iw::stdout_sink, iw::INFO);
		LOG_SINK(iw::stderr_sink, iw::ERR);
		LOG_SINK(iw::file_sink,         iw::INFO,  "/logs/sandbox_info.log");
		LOG_SINK(iw::file_sink,         iw::DEBUG, "/logs/sandbox_debug.log");

		// Events

		Bus->subscribe(iw::make_callback(&Application::HandleEvent, this));
		Bus->subscribe(iw::make_callback(&InputManager::HandleEvent, Input.get()));
		
		// Physics

		Physics->SetCollisionCallback(iw::make_callback(&Application::HandleCollision, this));

		// Asset Loader

		Asset->SetLoader<MeshLoader>();
		Asset->SetLoader<MaterialLoader>();
		Asset->SetLoader<TextureLoader>();
		Asset->SetLoader<ShaderLoader>();
		Asset->SetLoader<FontLoader>();

		// Space

		Space->SetEventBus(Bus);

		// Window

		m_window->SetEventbus(Bus);

		int status;
		LOG_DEBUG << "Initializing window...";
		if (status = m_window->Initialize(options.WindowOptions)) {
			LOG_ERROR << "Window failed to initialize with error code " << status;
			return status;
		}

		Renderer->Initialize();

		for (Layer* layer : m_layers) {
			LOG_DEBUG << "Initializing " << layer->Name() << " layer...";
			if (status = layer->Initialize()) {
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
		Bus->publish();
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

		LOG_DEBUG << "Started application";

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

		Renderer->Begin();

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

		ImGuiLayer* imgui = GetLayer<ImGuiLayer>("ImGui");

		// ImGui render (Sync)
		if (imgui) {
			imgui->Begin();
			for (Layer* layer : m_layers) {
				layer->ImGui();
			}
			imgui->End();
		}

		// Run through render queue! (Sync)

		Renderer->End();

		// Swap buffers (Sync)
		m_window->SwapBuffers();
		Console->ExecuteQueue(); 
		Bus->publish();
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
		if (e.Group == iw::val(EventGroup::INPUT)) {
			switch (e.Type) {
				case iw::val(InputEventType::Command): {
					InputCommandEvent& ice = e.as<InputCommandEvent>();
					Console->ExecuteCommand(ice.Command);
					e.Handled = true;
					break;
				}
				case iw::val(InputEventType::KeyTyped): {
					KeyTypedEvent& kte = e.as<KeyTypedEvent>();
					if (kte.Character == '/') {
						Console->QueueCommand(std::string(1, kte.Character));
						e.Handled = true;
						break;
					}
				}
			}
		}

		else if (e.Group == iw::val(EventGroup::WINDOW)) {
			switch (e.Type) {
				case Closed: {
					m_running = false;
					e.Handled = true;
					break;
				}
				case Resized: {
					WindowResizedEvent& wre = e.as<WindowResizedEvent>();
					Renderer->Width  = wre.Width;
					Renderer->Height = wre.Height;
					e.Handled = true;
					break;
				}
				default: {
					error = true;
					break;
				}
			}
		}

		if (error) {
			LOG_WARNING << "Application mishandled event " + e.Type << "!";
		}

		else {
			m_layers.DispatchEvent(e);
		}
	}

	bool Application::HandleCommand(
		const Command& command)
	{
		if (command.Verb == "resize_window") {
			Bus->push<WindowResizedEvent>(
				m_window->Id(),
				(int)command.Tokens[0].Int,
				(int)command.Tokens[1].Int);
		}

		return false;
	}

	void Application::HandleCollision(
		Manifold& manifold,
		scalar dt)
	{
		Bus->push<CollisionEvent>(
			manifold.BodyA, 
			manifold.BodyB, 
			manifold.PenetrationDepth, 
			dt);
	}
}
}
