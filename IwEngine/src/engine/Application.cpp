#include "iw/engine/Application.h"
#include "iw/engine/Layers/DebugLayer.h"
#include "iw/log/logger.h"
#include "iw/log/sink/file_sink.h"
#include "iw/events/callback.h"
#include "iw/graphics/Loaders/MeshLoader.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/graphics/Loaders/MaterialLoader.h"
#include "iw/graphics/Loaders/TextureLoader.h"
#include "iw/graphics/Loaders/ShaderLoader.h"
#include "iw/graphics/Loaders/FontLoader.h"
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
		: m_running       (false)
		, m_isInitialized (false)
	{

		Window = IWindow::Create();

		m_device = ref<IDevice>(IDevice::Create());
		Bus      = REF<eventbus>();
		Space    = REF<iw::Space>();
		Renderer = REF<QueuedRenderer>(REF<iw::Renderer>(m_device));
		Asset    = REF<AssetManager>();
		Input    = REF<InputManager>(Bus);
		Console  = REF<iw::Console>(make_getback(&Application::HandleCommand, this));
		Physics  = REF<DynamicsSpace>();
		Audio    = ref<IAudioSpace>(IAudioSpace::Create());
		Task     = REF<thread_pool>(std::thread::hardware_concurrency());
	}

	Application::~Application() {
		delete Window;
	}

	int Application::Initialize(
		InitOptions& options)
	{
		m_isInitialized = true; // techincally could fail and then this would be true, but needed for InitLayer

		//bool console = true; // should be in Initoptions, maybe should be creted in window also, this leaks windows.h
		//if (console) {
			AllocConsole();
			FILE* fo, * fe;
			freopen_s(&fo, "CONOUT$", "w", stdout);
			freopen_s(&fe, "CONERR$", "w", stderr);
		//}

		// if in 'export' mode should just use local asset folder...

		if (options.AssetRootPath == "")
		{
			options.AssetRootPath = IW_ASSET_ROOT_PATH;
		}

		Asset->SetRootPath(options.AssetRootPath);
		Audio->RootDirectory = Asset->RootPath() + "audio/";

		//PushLayerFront<DebugLayer>();

		// Time

		Time::UpdateTime();
		Time::SetFixedTime(1 / 50.0f); // default, should pass in options

		// Logging

#ifdef IW_DEBUG
		LOG_SINK(stdout_sink, loglevel::INFO);
		LOG_SINK(stderr_sink, loglevel::ERR);
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
		Physics->SetMultithread(Task);

		// Asset Loader

		Asset->SetLoader<MeshLoader>();
		Asset->SetLoader<ModelLoader>();
		Asset->SetLoader<MaterialLoader>();
		Asset->SetLoader<TextureLoader>();
		Asset->SetLoader<ShaderLoader>();
		Asset->SetLoader<FontLoader>();

		// Space

#ifdef IW_USE_EVENTS
		Space->SetEventBus(Bus);
#endif
		// Window

		Window->SetEventbus(Bus);

		int status;
		LOG_DEBUG << "Initializing window...";
		if (status = Window->Initialize(options.WindowOptions)) {
			LOG_ERROR << "Window failed to initialize with error code " << status;
			return status;
		}

		// Audio

		if (status = Audio->Initialize()) {
			LOG_ERROR << "Audio space failed to initialize with error code " << status;
			//return status;
		}

		Renderer->Now->SetWidth (options.WindowOptions.Width);
		Renderer->Now->SetHeight(options.WindowOptions.Height);
		Renderer->Now->Initialize();

		for (Layer* layer : m_layers) {
			if (status = InitLayer(layer)) {
				return status;
			}
		}

		//Need to set after so window doesn't send events before imgui gets initialized
		Window->SetState(options.WindowOptions.State);

		// Time again!

		Time::UpdateTime();

		// Thread pool

		Task->init();

		LOG_INFO << "Application initialized";

		return 0;
	}

	void Application::Run() {
		Bus->publish();
		Window->ReleaseOwnership();

		m_running = true;

		// Rendering thread is the 'main' thread for the application (owns rendering context)
		m_renderThread = std::thread([&]() {
			(*Window).TakeOwnership();
			
			float accumulatedTime = 0;
			while (m_running) {
				Time::UpdateTime();

				(*Renderer).Begin(Time::TotalTime());

				int layerNumber = 0;
				for (Layer* layer : m_layers)
				{
					LOG_TIME_SCOPE(layer->Name() + " layer pre update");
					(*Renderer).SetLayer(layerNumber);
					layer->PreUpdate();
				}

				if (Time::RawFixedTime() != 0.f)
				{
					int fixedIterations = 0;

					accumulatedTime += Time::DeltaTime();
					while (m_running
						&& accumulatedTime >= Time::RawFixedTime()
						&& fixedIterations < 10)
					{
						FixedUpdate();
						accumulatedTime -= Time::RawFixedTime(); // causes a runaway slowdown when physics update takes too long
						//accumulatedTime = 0;                       // not sure how to fix that, i think setting it to 0 is wrong tho so idk

						fixedIterations++;
					}
				}

				Update();

				layerNumber = 0;
				for (Layer* layer : m_layers)
				{
					LOG_TIME_SCOPE(layer->Name() + " layer post update");
					(*Renderer).SetLayer(layerNumber);
					layer->PostUpdate();
				}

				{
					LOG_TIME_SCOPE("Renderer");
					(*Renderer).End();
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
				{
					LOG_TIME_SCOPE("Audio");
					(*Audio).Update();
				}

				{
					LOG_TIME_SCOPE("Coroutines");
					(*Task).step_coroutines();
				}

				{
					LOG_TIME_SCOPE("Console & bus");
					(*Console).ExecuteQueue();
					(*Bus).publish();
				}

				(*Space).ExecuteQueue();

				(*Window).SwapBuffers();

				LOG_CLEAR_TIMES();
			}
		});

		LOG_DEBUG << "Started application";

		// Main loop gets events from os
		while (m_running) {
			Window->Update();
		}

		m_renderThread.join();
		
		Destroy();
	}

	void Application::Update() {
		// Start Work (ASync)
		
		//m_updateTask.Run();

		// Update layers (ASync)

		int layerNumber = 0;
		for (Layer* layer : m_layers)
		{
			LOG_TIME_SCOPE(layer->Name() + " layer update");
			Renderer->SetLayer(layerNumber);
			layer->UpdateSystems();
			layer->Update();
		}

		// Pause until work is finished (ASync)
		//m_updateTask.Wait();
	}

	void Application::FixedUpdate() {
		for (Layer* layer : m_layers) {
			layer->FixedUpdateSystems();
			layer->FixedUpdate();
		}

		LOG_TIME_SCOPE("Physics"); // this is going to have to be made an option, or another function that you can override to stop it so you can call it in a custon loop later
		Physics->Step(Time::FixedTime());
	}

	void Application::Destroy() {
		while (m_layers.size() > 0) {
			DestroyLayer(*m_layers.begin());
		}

		Window->Destroy();

		LOG_FLUSH();
		LOG_RESET();

		//if (console) {
			FreeConsole();
		//}
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
						Renderer->Now->Resize(wre.Width, wre.Height);
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
				Window->Id(),
				(int)command.Tokens[0].Int,
				(int)command.Tokens[1].Int);
		}

		else if (command.Verb == "quit") {
			Bus->push<WindowEvent>(Closed, Window->Id());
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
