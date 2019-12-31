#pragma once

#include "Core.h"
#include "Task.h"
#include "Window.h"
#include "Console.h"
#include "EventStack.h"
#include "InitOptions.h"
#include "Layers/ImGuiLayer.h"
#include "iw/entity/Space.h"
#include "iw/events/eventbus.h"
#include "iw/asset/AssetManager.h"
#include "iw/graphics/Renderer.h"
#include "iw/input/InputManager.h"
#include "iw/util/queue/blocking_queue.h"
#include "iw/physics/Dynamics/DynamicsSpace.h"
#include <vector>
#include <thread>

namespace IW {
namespace Engine {
	class Application {
	private:
		IWindow* m_window;
		iw::ref<IDevice> m_device;

		ImGuiLayer* m_imguiLayer;
		EventStack<Layer*> m_layers;

		std::thread m_renderThread;
		bool m_running;

	protected:
		iw::ref<Space>         Space;
		iw::ref<Renderer>      Renderer;
		iw::ref<AssetManager>  Asset;
		iw::ref<InputManager>  Input;
		iw::ref<Console>       Console;
		iw::ref<iw::eventbus>  Bus;
		iw::ref<DynamicsSpace> Physics;

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
		inline IWindow& GetWindow() {
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
			layer->SetApplicationVars(Space, Renderer, Asset, Physics, Bus);

			m_layers.PushBack(layer);
			return layer;
		}

		template<
			typename L,
			typename... Args>
		L* PushOverlay(
			Args&& ... args)
		{
			L* layer = new L(std::forward<Args>(args)...);
			layer->SetApplicationVars(Space, Renderer, Asset, Physics, Bus);

			m_layers.PushFront(layer);
			return layer;
		}

		template<
			typename L>
		void PopLayer(
			L* layer)
		{
			m_layers.Pop(layer);
		}

	};
}

	using namespace Engine;
}

IW::Application* CreateApplication(
	IW::InitOptions& options);
