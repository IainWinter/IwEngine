#pragma once

#include "Core.h"
#include "Window.h"
#include "EventStack.h"
#include "Task.h"
#include "iw/events/eventbus.h"
#include "InitOptions.h"
#include "Layers/ImGuiLayer.h"
#include "iw/asset/AssetManager.h"
#include "iw/input/InputManager.h"
#include "iw/entity/Space.h"
#include "iw/graphics/Renderer.h"
#include "iw/util/queue/blocking_queue.h"
#include <vector>
#include <thread>

namespace IwEngine {
	class IWENGINE_API Application {
	private:
		bool                 m_running;
		IWindow*             m_window;
		ImGuiLayer*          m_imguiLayer;
		EventStack<Layer*>   m_layers;

		iwu::ref<IW::IDevice> m_device;

		std::thread  m_renderThread;
		Task<void()> m_updateTask;

	protected:
		IwEntity::Space Space;
		IwInput::InputManager InputManager;

		iw::eventbus Bus;

		IW::Renderer     Renderer;
		IW::AssetManager Asset;

	public:
		Application();
		virtual ~Application();

		virtual int Initialize(
			InitOptions& options);

		virtual void Run();
		virtual void Destroy();
		virtual void Update();
		virtual void FixedUpdate();
		//virtual void Pause();

		virtual void HandleEvent(
			iw::event& e);

		inline IWindow& GetWindow() {
			return *m_window;
		}

		template<
			typename L,
			typename... Args>
		L* PushLayer(
			Args&&... args)
		{
			L* layer = new L(Space, Renderer, Asset, std::forward<Args>(args)...);
			m_layers.PushBack(layer);
			return layer;
		}

		template<
			typename L,
			typename... Args>
		L* PushOverlay(
				Args&& ... args)
		{
			L* layer = new L(Space, Renderer, Asset, std::forward<Args>(args)...);
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

IwEngine::Application* CreateApplication(
	IwEngine::InitOptions& options);
