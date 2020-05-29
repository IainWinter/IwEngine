#include "iw/engine/Application.h"
#include "iw/engine/Layers/DebugLayer.h"
#include "iw/engine/Time.h"
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
		Renderer = REF<iw::QueuedRenderer>(m_device);
		Asset    = REF<AssetManager>();
		Input    = REF<InputManager>(Bus);
		Console  = REF<iw::Console>(iw::make_getback(&Application::HandleCommand, this));
		Physics  = REF<DynamicsSpace>();
		Audio    = REF<AudioSpaceRaw>("assets/sounds/");

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
		LOG_SINK(iw::stdout_sink, iw::INFO);
		LOG_SINK(iw::stderr_sink, iw::ERR);
#else
		LOG_SINK(iw::async_stdout_sink, iw::INFO);
		LOG_SINK(iw::async_stderr_sink, iw::ERR);
#endif
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

		Asset->SetLoader<AssetLoader<Mesh>>(); // make asset store that asset loader extends loading abilities to

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

		// Audio

		if (status = Audio->Initialize()) {
			LOG_ERROR << "Audio space failed to initialize with error code " << status;
			return status;
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

				if (Time::RawFixedTime() == 0)
					continue;

				accumulatedTime += Time::DeltaTime();
				while (m_running 
					&& accumulatedTime >= Time::RawFixedTime())
				{
					FixedUpdate();
					accumulatedTime -= Time::RawFixedTime();
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

	std::unordered_map<const char*, float> update_times;
	std::unordered_map<const char*, float> post_update_times;
	std::unordered_map<const char*, std::unordered_map<const char*, float>> system_update_times;
	float renderTime;
	float eventTime;
	float physicsTime;
	size_t ticks;
	float smooth = 0.004f;

	void Application::Update() {
		// Update time (Sync)
		Time::UpdateTime();

		Renderer->Begin();

		// Clear window and poll events (Sync)
		//m_window->Clear();

		//m_workQueue.clear();

		// Pre Update (Sync)
		 
		int layerNumber = 0;

		for (Layer* layer : m_layers) {
			float start = iw::DeltaTimeNow();

			Renderer->SetLayer(layerNumber);
			layer->PreUpdate();
		}

		// Start Work (ASync)
		//m_updateTask.Run();

		// Update layers (ASync)

		layerNumber = 0;

		for (Layer* layer : m_layers) {
			float start = iw::Time::DeltaTimeNow();

			Renderer->SetLayer(layerNumber);
			layer->UpdateSystems(system_update_times[layer->Name()], smooth);
			layer->Update();

			float end = iw::Time::DeltaTimeNow();
			update_times[layer->Name()] = iw::lerp(update_times[layer->Name()], end - start, smooth);
		}

		// Pause until work is finished (ASync)
		//m_updateTask.Wait();

		// Post Update (Sync)

		layerNumber = 0;

		for (Layer* layer : m_layers) {
			float start = iw::Time::DeltaTimeNow();

			Renderer->SetLayer(layerNumber);
			layer->PostUpdate();

			float end = iw::Time::DeltaTimeNow();
			post_update_times[layer->Name()] = iw::lerp(post_update_times[layer->Name()], end - start, smooth);
		}

		// Run through render queue! (Sync)

		float renderStart = iw::Time::DeltaTimeNow();

		Renderer->End();

		float renderEnd = iw::Time::DeltaTimeNow();
		renderTime = iw::lerp(renderTime, renderEnd - renderStart, smooth);

		// ImGui render (Sync)

		ImGuiLayer* imgui = GetLayer<ImGuiLayer>("ImGui");
		if (imgui) {
			imgui->Begin();
			for (Layer* layer : m_layers) {
				layer->ImGui();
			}

			ImGui::Begin("Times");
			ImGui::SliderFloat("Smooth", &smooth, 0, 1);

			ImGui::Text("Tick %i", iw::Time::Ticks());
			ImGui::Text("Renderer took %4.4f ms", renderTime  * 1000/*/ (iw::Time::Ticks() - ticks)*/);
			ImGui::Text("Physics  took %4.4f ms", physicsTime * 1000/*/ (iw::Time::Ticks() - ticks)*/);
			ImGui::Text("Eventbus took %4.4f ms", eventTime   * 1000/*/ (iw::Time::Ticks() - ticks)*/);

			for (Layer* layer : m_layers) {
				ImGui::Text("%4.4f ms - %s layer",   post_update_times[layer->Name()] * 1000/*/ (iw::Time::Ticks() - ticks)*/, layer->Name());
				ImGui::Text("%4.4f ms - %s systems", update_times     [layer->Name()] * 1000/*/ (iw::Time::Ticks() - ticks)*/, layer->Name());

				for (ISystem* system : layer->temp_GetSystems()) {
					ImGui::Text("\t%4.4f ms - %s system", system_update_times[layer->Name()][system->Name()] * 1000/*/ (iw::Time::Ticks() - ticks)*/, system->Name());
				}
			}
			ImGui::End();

			imgui->End();
		}

		/*if (iw::Time::Ticks() % 100 == 0) {
			ticks = iw::Time::Ticks() - 1;
			for (auto p : update_times) {
				p.second /= 100;
			}

			for (auto p : post_update_times) {
				p.second /= 100;
			}

			for (auto p : system_update_times) {
				for (auto q : p.second) {
					q.second /= 100;
				}
			}
		}*/

		Audio->Update();

		float eventStart = iw::Time::DeltaTimeNow();

		Console->ExecuteQueue();
		Bus->publish();

		float eventEnd = iw::Time::DeltaTimeNow();
		eventTime = iw::lerp(eventTime, eventEnd - eventStart, smooth);
		
		// Swap buffers (Sync)
		
		m_window->SwapBuffers();
	}

	void Application::FixedUpdate() {
		for (Layer* layer : m_layers) {
			layer->FixedUpdateSystems();
			layer->FixedUpdate();
		}

		float physicsStart = iw::Time::DeltaTimeNow();
		
		Physics->Step(iw::Time::FixedTime());
		
		float physicsEnd = iw::Time::DeltaTimeNow();
		physicsTime = iw::lerp(physicsTime, physicsEnd - physicsStart, smooth);
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
					Console->QueueCommand(ice.Command);
					e.Handled = true;
					break;
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
			manifold.ObjA, 
			manifold.ObjB, 
			manifold.PenetrationDepth, 
			dt);
	}
}
}
