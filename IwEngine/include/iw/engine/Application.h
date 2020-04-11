#pragma once

#include "Core.h"
#include "Task.h"
#include "Window.h"
#include "Console.h"
#include "EventStack.h"
#include "InitOptions.h"
#include "Layers/ImGuiLayer.h"
#include "iw/entity/Space.h"
#include "iw/audio/AudioSpace.h"
#include "iw/physics/Dynamics/DynamicsSpace.h"
#include "iw/events/eventbus.h"
#include "iw/graphics/QueuedRenderer.h"
#include "iw/asset/AssetManager.h"
#include "iw/input/InputManager.h"
#include "iw/util/queue/blocking_queue.h"
#include <vector>
#include <thread>

namespace iw {
namespace Engine {
	class Application {
	private:
		IWindow* m_window;
		ref<IDevice> m_device;

		EventStack<Layer*> m_layers;

		std::thread m_renderThread;
		bool m_running;

	protected:
		ref<Space>          Space;
		ref<iw::QueuedRenderer> Renderer;
		ref<AssetManager>   Asset;
		ref<InputManager>   Input;
		ref<Console>        Console;
		ref<DynamicsSpace>  Physics;
		ref<AudioSpace>     Audio;
		ref<eventbus>       Bus;

	public:
		IWENGINE_API
		Application();

		IWENGINE_API
		virtual ~Application();

		IWENGINE_API
		virtual int Initialize(
			InitOptions& options);

		IWENGINE_API
		virtual void Run();

		IWENGINE_API
		virtual void Destroy();

		IWENGINE_API
		virtual void Update();

		IWENGINE_API
		virtual void FixedUpdate();
		//virtual void Pause();

		IWENGINE_API
		virtual void HandleEvent(
			iw::event& e);

		IWENGINE_API
		virtual bool HandleCommand(
			const Command& command);

		IWENGINE_API
		virtual void HandleCollision(
			Manifold& manifold,
			scalar dt);

		IWENGINE_API
		inline IWindow& Window() {
			return *m_window;
		}

		template<
			typename L = Layer>
		L* GetLayer(
			const char* name)
		{
			return (L*)m_layers.Get(name);
		}

		template<
			typename L,
			typename... Args>
		L* PushLayer(
			Args&&... args)
		{
			L* layer = new L(std::forward<Args>(args)...);
			layer->SetApplicationVars(Space, Renderer, Asset, Physics, Audio, Bus);

			PushLayer(layer);
			return layer;
		}

		template<
			typename L,
			typename... Args>
		L* PushOverlay(
			Args&& ... args)
		{
			L* layer = new L(std::forward<Args>(args)...);
			layer->SetApplicationVars(Space, Renderer, Asset, Physics, Audio, Bus);

			PushOverlay(layer);
			return layer;
		}

		template<
			typename L>
		void PushLayer(
			L* layer)
		{
			LOG_INFO << "Pushed " << layer->Name() << " layer";
			layer->SetApplicationVars(Space, Renderer, Asset, Physics, Audio, Bus);
			m_layers.PushBack(layer);

			layer->OnPush();
		}

		template<
			typename L>
		void PushOverlay(
			L* layer)
		{
			LOG_INFO << "Pushed " << layer->Name() << " overlay";
			layer->SetApplicationVars(Space, Renderer, Asset, Physics, Audio, Bus);
			m_layers.PushFront(layer);

			layer->OnPush();
		}

		template<
			typename L>
		void PopLayer(
			L* layer)
		{
			LOG_INFO << "Popped " << layer->Name() << " layer";
			m_layers.Pop(layer);

			layer->OnPop();
		}
	};
}

	using namespace Engine;
}

iw::Application* CreateApplication(
	iw::InitOptions& options);
