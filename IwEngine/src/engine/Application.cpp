#include "iw/engine/Application.h"
#include "iw/engine/Layers/DebugLayer.h"
#include "iw/log/logger.h"
#include "iw/log/sink/file_sink.h"
#include "iw/events/callback.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/graphics/Loaders/MaterialLoader.h"
#include "iw/graphics/Loaders/TextureLoader.h"
#include "iw/graphics/Loaders/ShaderLoader.h"
#include "iw/graphics/Loaders/FontLoader.h"
#include "iw/audio/AudioSpaceRaw.h"
#include <atomic>

#ifdef IW_IMGUI
#	include "iw/engine/Layers/ImGuiLayer.h"
#endif

#ifdef IW_DEBUG
	#include "iw/log/sink/std_sink.h"
#else
	#include "iw/log/sink/async_std_sink.h"
#endif

namespace iw {
namespace Engine {
	Application::Application()
		: m_running(false)
		, m_window(IWindow::Create())
	{
		m_device = ref<IDevice>(IDevice::Create());
		Bus      = REF<eventbus>();
		Space    = REF<iw::Space>();
		Renderer = REF<QueuedRenderer>(m_device);
		Asset    = REF<AssetManager>();
		Input    = REF<InputManager>(Bus);
		Console  = REF<iw::Console>(make_getback(&Application::HandleCommand, this));
		Physics  = REF<DynamicsSpace>();
		Audio    = REF<AudioSpaceRaw>("assets/sounds/");
		Task     = REF<thread_pool>(std::thread::hardware_concurrency());

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

#ifdef IW_DEBUG
		LOG_SINK(stdout_sink, INFO);
		LOG_SINK(stderr_sink, ERR);
#else
		LOG_SINK(async_stdout_sink, loglevel::INFO);
		LOG_SINK(async_stderr_sink, loglevel::ERR);
#endif
		LOG_SINK(file_sink,         loglevel::INFO,  "/logs/sandbox_info.log");
		LOG_SINK(file_sink,         loglevel::DEBUG, "/logs/sandbox_debug.log");

		LOG_SET_GET_TIME(Time::DeltaTimeNow);

		// Events

		Bus->subscribe(make_callback(&Application::HandleEvent, this));
		Bus->subscribe(make_callback(&InputManager::HandleEvent, Input.get()));
		
		// Physics

		Physics->SetCollisionCallback(make_callback(&Application::HandleCollision, this));

		// Asset Loader

		Asset->SetLoader<MeshLoader>();
		Asset->SetLoader<MaterialLoader>();
		Asset->SetLoader<TextureLoader>();
		Asset->SetLoader<ShaderLoader>();
		Asset->SetLoader<FontLoader>();

		Asset->SetLoader<AssetLoader<Mesh>>(); // make asset store that asset loader extends loading abilities to

		// Space

#ifdef IW_USE_EVENTS
		Space->SetEventBus(Bus);
#endif
		// Window

		m_window->SetEventbus(Bus);

		int status;
		LOG_DEBUG << "Initializing window...";
		if (status = m_window->Initialize(options.WindowOptions)) {
			LOG_ERROR << "Window failed to initialize with error code " << status;
			return status;
		}

		// Audio

		if (status = Audio->Initialize()) {
			LOG_ERROR << "Audio space failed to initialize with error code " << status;
			//return status;
		}

		Renderer->SetWidth (options.WindowOptions.Width);
		Renderer->SetHeight(options.WindowOptions.Height);
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
		Time::SetFixedTime(1 / 50.0f);

		// Thread pool

		Task->init();

		LOG_INFO << "Application initialized";

		return 0;
	}

	void Application::Run() {
		Bus->publish();
		m_window->ReleaseOwnership();

		m_running = true;

		// Rendering thread is the 'main' thread for the application (owns rendering context)
		m_renderThread = std::thread([&]() {
			m_window->TakeOwnership();
			
			float accumulatedTime = 0;
			while (m_running) {
				Time::UpdateTime();

				(*Renderer).Begin(Time::TotalTime());

				Update();

				if (Time::RawFixedTime() == 0)
					continue;

				accumulatedTime += Time::DeltaTime();
				while (m_running
					&& accumulatedTime >= Time::RawFixedTime())
				{
					FixedUpdate();
					accumulatedTime -= Time::RawFixedTime();
				}

				{
					LOG_TIME_SCOPE("Renderer");
					(*Renderer).End();
				}

				{
					LOG_TIME_SCOPE("Console & bus");
					(*Console).ExecuteQueue();
					(*Bus).publish();
				}

				(*Space).ExecuteQueue();

				{
					LOG_TIME_SCOPE("Audio");
					(*Audio).Update();
				}

#ifdef IW_IMGUI
				ImGuiLayer* imgui = GetLayer<ImGuiLayer>("ImGui");
				if (imgui) {
					LOG_TIME_SCOPE("ImGui");

					imgui->Begin();
					for (Layer* layer : m_layers) {
						layer->ImGui();
					}
					imgui->End();
				}
#endif

				m_window->SwapBuffers();

				LOG_CLEAR_TIMES();
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
		// Pre Update (Sync)
		 
		int layerNumber = 0;

		for (Layer* layer : m_layers) {
			LOG_TIME_SCOPE(layer->Name() + " layer pre update");

			Renderer->SetLayer(layerNumber);
			layer->PreUpdate();
		}

		// Start Work (ASync)
		//m_updateTask.Run();

		// Update layers (ASync)

		layerNumber = 0;

		for (Layer* layer : m_layers) {
			LOG_TIME_SCOPE(layer->Name() + " layer update");

			Renderer->SetLayer(layerNumber);
			layer->UpdateSystems();
			layer->Update();
		}

		// Pause until work is finished (ASync)
		//m_updateTask.Wait();

		// Post Update (Sync)

		layerNumber = 0;

		for (Layer* layer : m_layers) {
			LOG_TIME_SCOPE(layer->Name() + " layer post update");

			Renderer->SetLayer(layerNumber);
			layer->PostUpdate();
		}
	}

	void Application::FixedUpdate() {
		for (Layer* layer : m_layers) {
			layer->FixedUpdateSystems();
			layer->FixedUpdate();
		}

		LOG_TIME_SCOPE("Physics");
		Physics->Step(Time::FixedTime());
	}

	void Application::Destroy() {
		m_window->Destroy();
		for (Layer* layer : m_layers) {
			layer->Destroy();
		}
	}

	void Application::HandleEvent(
		event& e)
	{
		bool error = false;
		switch (e.Group) {
			case val(EventGroup::INPUT): {
				switch (e.Type) {
					case val(InputEventType::Command): {
						InputCommandEvent& ice = e.as<InputCommandEvent>();
						Console->QueueCommand(ice.Command);
						e.Handled = true;
						break;
					}
				}

				break;
			}
			case val(EventGroup::WINDOW): {
				switch (e.Type) {
					case Closed: {
						m_running = false;
						e.Handled = true;
						break;
					}
					case Resized: {
						WindowResizedEvent& wre = e.as<WindowResizedEvent>();
						Renderer->Resize(wre.Width, wre.Height);
						e.Handled = true;
						break;
					}
					default: {
						error = true;
						break;
					}
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

		else if (command.Verb == "quit") {
			Bus->push<WindowEvent>(Closed, m_window->Id());
		}

		return false;
	}

	void Application::HandleCollision(
		Manifold& manifold,
		scalar dt)
	{
		Bus->push<CollisionEvent>(manifold, dt);
	}
}
}
